#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/slab.h>

MODULE_LICENSE("GLP");
MODULE_AUTHOR("Louis Solofrizzo <louis@ne02ptzero.me>");
MODULE_DESCRIPTION("Useless module");

static ssize_t myfd_read(struct file *fp, char __user *user,
		size_t size, loff_t *offs);
static ssize_t myfd_write(struct file *fp, const char __user *user,
		size_t size, loff_t *offs);

static struct file_operations myfd_fops = {
	.owner = THIS_MODULE,
	.read = &myfd_read,
	.write = &myfd_write
};

static struct miscdevice myfd_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "reverse",
	.fops = &myfd_fops
};

char str[PAGE_SIZE];
char *tmp;

static int __init myfd_init(void)
{
	int retval;

	retval = misc_register(&myfd_device);
	if (retval != 0) {
		pr_err("Could not register misc device\n");
		return retval;
	}
	return 0;
}

static void __exit myfd_cleanup(void)
{
	misc_deregister(&myfd_device);
}

ssize_t myfd_read(struct file *fp, char __user *user, size_t size,
		loff_t *offs)
{
	return simple_read_from_buffer(user, size, offs, str, PAGE_SIZE);
}

ssize_t myfd_write(struct file *fp, const char __user *user, size_t size,
		loff_t *offs)
{
	ssize_t res;

	res = simple_write_to_buffer(str, PAGE_SIZE, offs, user, size);
	if (res < PAGE_SIZE) {
		str[res + 1] = 0;
		return res + 1;
	}
	str[PAGE_SIZE - 1] = 0;
	return res;
}

module_init(myfd_init);
module_exit(myfd_cleanup);
