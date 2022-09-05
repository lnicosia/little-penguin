#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

int	main(int ac, char **av)
{
	char	buf[1024];
	ssize_t	ret;
	int		fd;

	if ((fd = open("/dev/fortytwo", O_RDWR)) == -1) {
		perror("open on /dev/fortytwo");
		return -1;
	}

	/*	Reading	*/
	memset(buf, 0, 1024);
	printf("Reading 1024 bytes /dev/fortytwo...\n");
	if ((ret = read(fd, buf, 1024)) == -1)
	{
		perror("read on /dev/fortytwo");
		goto close_fd;
	}
	printf("Ret = %ld\n", ret);
	if (ret > 0 && ret != 1024)
		buf[ret] = 0;
	printf("File contains '%s'\n", buf);

	close(fd);
	fd = open("/dev/fortytwo", O_RDWR);
	/*	Reading 1 byte	*/
	for (int i = 0; i < 10; i++) {
		printf("Reading 1 byte /dev/fortytwo...\n");
		memset(buf, 0, 1024);
		if ((ret = read(fd, buf, 1)) == -1)
		{
			perror("read on /dev/fortytwo");
			goto close_fd;
		}
		printf("Ret = %ld\n", ret);
		if (ret > 0 && ret != 2)
			buf[ret] = 0;
		printf("File contains '%s'\n", buf);
	}
	/*	Writing	*/
	printf("Writing in /dev/fortytwo...\n");

	printf("Writing toto..\n");
	printf("Ret = %d\n", write(fd, "toto", 4));

	printf("Writing lnicosia..\n");
	printf("Ret = %d\n", write(fd, "lnicosia", 8));

	printf("Writing lnicosi..\n");
	printf("Ret = %d\n", write(fd, "lnicosi", 7));

	printf("Writing lnicosiaa..\n");
	printf("Ret = %d\n", write(fd, "lnicosiaa", 9));
close_fd:
	close(fd);
	return 0;
}
