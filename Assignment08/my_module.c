// SPDX-License-Identifier: GPL

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Louis Solofrizzo <louis@ne02ptzero.me>");
MODULE_DESCRIPTION("Useless module");

static ssize_t myfd_read(struct file *fp, char __user *user,
			 size_t size, loff_t *offs);
static ssize_t myfd_write(struct file *fp, const char __user *user,
			  size_t size, loff_t *offs);

static const struct file_operations myfd_fops = {
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
char tmp[PAGE_SIZE];

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
	size_t t;
	size_t i;
	size_t len;

	len = strlen(str);
	if (len == 0)
		return 0;
	t = len - 1;
	i = 0;
	while (i < len) {
		tmp[i] = str[t];
		t--;
		i++;
	}
	tmp[i] = 0;
	return simple_read_from_buffer(user, size, offs, tmp, i);
}

ssize_t myfd_write(struct file *fp, const char __user *user, size_t size,
		   loff_t *offs)
{
	ssize_t res;

	res = simple_write_to_buffer(str, PAGE_SIZE, offs, user, size);
	if (res >= 0 && res < PAGE_SIZE) {
		str[res] = 0;
		return res;
	}
	str[PAGE_SIZE - 1] = 0;
	return res;
}

module_init(myfd_init);
module_exit(myfd_cleanup);
