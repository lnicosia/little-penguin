#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

int	main(int ac, char **av)
{
	char	buf[1024];
	ssize_t	ret;
	int	id_fd;
	int	jiffies_fd;
	int	foo_fd;
	uid_t	id;

	setuid(0);
	seteuid(0);
	id = getuid();
	if (id == 1000)
		printf("Logged as lucas\n");
	else if (id == 0)
		printf("Logged as root\n");

	/*	Open /sys/kernel/debug/id */
	if ((id_fd = open("/sys/kernel/debug/fortytwo/id", O_RDWR)) == -1) {
		perror("open on /sys/kernel/debug/fortytwo/id");
		return -1;
	}
	printf("/sys/kernel/debug/fortytwo/id fd = %d\n", id_fd);
	/*	Open /sys/kernel/debug/jiffies */
	if ((jiffies_fd = open("/sys/kernel/debug/fortytwo/jiffies", O_RDONLY)) == -1) {
		perror("open on /sys/kernel/debug/fortytwo/jiffies");
		goto close_id_fd;
	}
	printf("/sys/kernel/debug/fortytwo/jiffies fd = %d\n", jiffies_fd);
	/*	Open /sys/kernel/debug/foo */
	mode_t mode = id == 0 ? O_RDWR : O_RDONLY;
	if ((foo_fd = open("/sys/kernel/debug/fortytwo/foo", mode)) == -1) {
		perror("open on /sys/kernel/debug/fortytwo/foo");
		goto close_jiffies_fd;
	}
	printf("/sys/kernel/debug/fortytwo/foo fd = %d\n", foo_fd);

	printf("\n--Reading--\n");
	/*	Reading id */
	printf("Reading /sys/kernel/debug/fortytwo/id...\n");
	if ((ret = read(id_fd, buf, 1024)) == -1)
	{
		perror("read on /sys/kernel/debug/fortytwo/id");
		goto close_foo_fd;
	}
	if (ret > 0 && ret != 1024)
		buf[ret] = 0;
	printf("File contains '%s'\n", buf);

	/*	Reading jiffies */
	printf("Reading /sys/kernel/debug/fortytwo/jiffies...\n");
	if ((ret = read(jiffies_fd, buf, 1024)) == -1)
	{
		perror("read on /sys/kernel/debug/fortytwo/jiffies");
		goto close_foo_fd;
	}
	if (ret > 0 && ret != 1024)
		buf[ret] = 0;
	printf("File contains '%s'\n", buf);

	/*	Reading foo */
	printf("Reading /sys/kernel/debug/fortytwo/foo...\n");
	if ((ret = read(foo_fd, buf, 1024)) == -1)
	{
		perror("read on /sys/kernel/debug/fortytwo/foo");
		goto close_foo_fd;
	}
	if (ret > 0 && ret != 1024)
		buf[ret] = 0;
	printf("File contains '%s'\n", buf);

	/*	Writing	*/
	printf("\n--Writing--\n");
	printf("Writing in /sys/kernel/debug/fortytwo/id...\n");

	printf("Writing toto..\n");
	printf("Ret = %d\n", write(id_fd, "toto", 4));

	printf("Writing lnicosia..\n");
	printf("Ret = %d\n", write(id_fd, "lnicosia", 8));

	printf("Writing lnicosi..\n");
	printf("Ret = %d\n", write(id_fd, "lnicosi", 7));

	printf("Writing lnicosiaa..\n");
	printf("Ret = %d\n", write(id_fd, "lnicosiaa", 9));

	printf("\nWriting in /sys/kernel/debug/fortytwo/foo...\n");

	printf("Writing toto..\n");
	write(foo_fd, "toto", 4);
	/*	Reading foo */
	printf("Reading /sys/kernel/debug/fortytwo/foo...\n");
	if ((ret = read(foo_fd, buf, 1024)) == -1)
	{
		perror("read on /sys/kernel/debug/fortytwo/foo");
		goto close_foo_fd;
	}
	if (ret > 0 && ret != 1024)
		buf[ret] = 0;
	printf("File contains '%s'\n", buf);

	/*	Page size tests	*/
#define BUF_SIZE 400
	char buff_ok[BUF_SIZE];
	memset(buff_ok, 'a', BUF_SIZE - 1);
	buff_ok[4094] = 0;
	char buff_ko[4097];
	memset(buff_ko, 'o', 4097);
	printf("Writing %d 'a'. Ret = %d\n", BUF_SIZE, write(foo_fd, buff_ok, BUF_SIZE));
	/*	Reading foo */
	printf("Reading /sys/kernel/debug/fortytwo/foo...\n");
	if ((ret = read(foo_fd, buf, 1024)) == -1)
	{
		perror("read on /sys/kernel/debug/fortytwo/foo");
		goto close_foo_fd;
	}
	if (ret > 0 && ret != 1024)
		buf[ret] = 0;
	printf("File contains '%s'\n", buf);
	//printf("Writing 4097 'o'. Ret = %d\n", write(foo_fd, buff_ko, 4097));
	/*	Reading foo */
	/*printf("Reading /sys/kernel/debug/fortytwo/foo...\n");
	if ((ret = read(foo_fd, buf, 1024)) == -1)
	{
		perror("read on /sys/kernel/debug/fortytwo/foo");
		goto close_foo_fd;
	}
	if (ret > 0 && ret != 1024)
		buf[ret] = 0;
	printf("File contains '%s'\n", buf);*/
close_foo_fd:
	printf("/sys/kernel/debug/fortytwo/foo fd = %d\n", foo_fd);
	close(foo_fd);
close_jiffies_fd:
	printf("/sys/kernel/debug/fortytwo/jiffies fd = %d\n", jiffies_fd);
	close(jiffies_fd);
close_id_fd:
	printf("/sys/kernel/debug/fortytwo/id fd = %d\n", id_fd);
	close(id_fd);
	return -0;
}
