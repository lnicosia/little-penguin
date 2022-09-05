#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#define PAGE_SIZE 4096
int	main(int ac, char **av)
{
	char	buf[PAGE_SIZE];
	ssize_t	ret;
	int	id_fd;
	int	jiffies_fd;
	int	foo_fd;
	uid_t	id;

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
	if ((foo_fd = open("/sys/kernel/debug/fortytwo/foo", O_RDWR)) == -1) {
		perror("open on /sys/kernel/debug/fortytwo/foo");
		goto close_jiffies_fd;
	}
	printf("/sys/kernel/debug/fortytwo/foo fd = %d\n", foo_fd);

	printf("\n--Reading--\n");
	/*	Reading id */
	bzero(buf, PAGE_SIZE);
	printf("Reading /sys/kernel/debug/fortytwo/id...\n");
	if ((ret = read(id_fd, buf, PAGE_SIZE)) == -1)
	{
		perror("read on /sys/kernel/debug/fortytwo/id");
		goto close_foo_fd;
	}
	if (ret > 0 && ret != PAGE_SIZE)
		buf[ret] = 0;
	printf("File contains '%s'\n", buf);
	close(id_fd);
	id_fd = open("/sys/kernel/debug/fortytwo/id", O_RDWR);
	for (int i = 0; i < 10; i++) {
		bzero(buf, PAGE_SIZE);
		printf("Reading 1 byte in /sys/kernel/debug/fortytwo/id...\n");
		if ((ret = read(id_fd, buf, 1)) == -1)
		{
			perror("read on /sys/kernel/debug/fortytwo/id");
			goto close_foo_fd;
		}
		if (ret > 0 && ret != PAGE_SIZE)
			buf[ret] = 0;
		buf[PAGE_SIZE] = 0;
		printf("File contains '%s'\n", buf);
	}

	/*	Reading jiffies */
	printf("Reading /sys/kernel/debug/fortytwo/jiffies...\n");
	bzero(buf, PAGE_SIZE);
	if ((ret = read(jiffies_fd, buf, PAGE_SIZE)) == -1)
	{
		perror("read on /sys/kernel/debug/fortytwo/jiffies");
		goto close_foo_fd;
	}
	if (ret > 0 && ret != PAGE_SIZE)
		buf[ret] = 0;
	printf("File contains '%s'\n", buf);

	/*	Reading foo */
	printf("Reading /sys/kernel/debug/fortytwo/foo...\n");
	bzero(buf, PAGE_SIZE);
	if ((ret = read(foo_fd, buf, PAGE_SIZE)) == -1)
	{
		perror("read on /sys/kernel/debug/fortytwo/foo");
		goto close_foo_fd;
	}
	if (ret > 0 && ret != PAGE_SIZE)
		buf[ret] = 0;
	buf[PAGE_SIZE - 1] = 0;
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
	bzero(buf, PAGE_SIZE);
	printf("Reading /sys/kernel/debug/fortytwo/foo...\n");
	close(foo_fd);
	foo_fd = open("/sys/kernel/debug/fortytwo/foo", O_RDWR);
	if ((ret = read(foo_fd, buf, PAGE_SIZE)) == -1)
	{
		perror("read on /sys/kernel/debug/fortytwo/foo");
		goto close_foo_fd;
	}
	if (ret > 0 && ret != PAGE_SIZE)
		buf[ret] = 0;
	buf[PAGE_SIZE - 1] = 0;
	printf("ret = %d\n", ret);
	printf("File contains '%s'\n", buf);

	/*	Page size tests	*/
#define BUF_SIZE PAGE_SIZE * 2
	char buff_ko[BUF_SIZE];
	memset(buff_ko, 'a', BUF_SIZE);
	printf("Writing %d 'a'. Ret = %d\n",
			BUF_SIZE, write(foo_fd, buff_ko, BUF_SIZE));
	/*	Reading foo */
	bzero(buf, PAGE_SIZE);
	printf("Reading /sys/kernel/debug/fortytwo/foo...\n");
	close(foo_fd);
	foo_fd = open("/sys/kernel/debug/fortytwo/foo", O_RDWR);
	if ((ret = read(foo_fd, buf, PAGE_SIZE)) == -1)
	{
		perror("read on /sys/kernel/debug/fortytwo/foo");
		goto close_foo_fd;
	}
	if (ret > 0 && ret != PAGE_SIZE)
		buf[ret] = 0;
	buf[PAGE_SIZE - 1] = 0;
	printf("ret = %d\n", ret);
	printf("File contains '%s'\n", buf);

	/*	Opening foo in RDWR as lucas	*/
	close(foo_fd);
	setuid(1000);
	seteuid(1000);
	printf("Opening /sys/kernel/debug/fortytwo/foo in RDWR as lucas (uid = 1000)\n");
	if ((foo_fd = open("/sys/kernel/debug/fortytwo/foo", O_RDWR)) == -1)
		perror("open on /sys/kernel/debug/fortytwo/foo");
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
