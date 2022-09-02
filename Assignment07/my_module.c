#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/debugfs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lucas Nicosia <lnicosia@student.42.fr>");
MODULE_DESCRIPTION("Debugfs stuff.");

struct dentry *ddir;

struct dentry *did;
struct dentry *djiffies;
struct dentry *dfoo;

char foo_data[PAGE_SIZE];

static ssize_t write_student_login(struct file *file, const char __user *buf,
		size_t len, loff_t *ppos)
{
	size_t cmp_len = len < 8 ? 8 : len;
	if (strncmp("lnicosia", buf, cmp_len) == 0)
		return strlen("lnicosia");
	return -EINVAL;
}

static ssize_t read_student_login(struct file *filp, char __user *buf,
		size_t count, loff_t *f_pos)
{
	return simple_read_from_buffer(buf, count, f_pos, "lnicosia\0", 8);
}

static ssize_t read_jiffies_timer(struct file *filp, char __user *buf,
		size_t count, loff_t *f_pos)
{
	char local_buf[16];
	int len;

	len = sprintf(local_buf, "%ld", jiffies);
	if (len < 16)
		local_buf[len] = 0;
	return simple_read_from_buffer(buf, count, f_pos, local_buf, len);
}

static ssize_t write_foo(struct file *file, const char __user *buf,
		size_t len, loff_t *ppos)
{
	pr_info("Len = %ld\n", len);
	if (len > PAGE_SIZE) {
		pr_err("Writing too much data!\n");
		return -1;
	}
	return simple_write_to_buffer(foo_data, len, ppos, buf, len);
}

static ssize_t read_foo(struct file *filp, char __user *buf,
		size_t count, loff_t *f_pos)
{
	size_t len = strlen(foo_data);
	return simple_read_from_buffer(buf, count, f_pos, foo_data, len);
}

static const struct file_operations login_fops = {
	.owner = THIS_MODULE,
	.write = write_student_login,
	.read = read_student_login
};

static const struct file_operations jiffies_fops = {
	.owner = THIS_MODULE,
	.read = read_jiffies_timer
};

static const struct file_operations foo_fops = {
	.owner = THIS_MODULE,
	.write = write_foo,
	.read = read_foo
};

static int __init debugfs_stuff_init(void)
{
   	if ((ddir = debugfs_create_dir("fortytwo", NULL)) == NULL) {
		pr_err("Cannot create debugfs directory\n");
		return -1;
	}
   	if ((did = debugfs_create_file("id",
			S_IRWXU | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH,
			ddir, NULL, &login_fops)) == NULL) {
		pr_err("Cannot create id file\n");
		goto remove_debugfs;
	}
   	if ((djiffies = debugfs_create_file("jiffies",
			S_IRUSR | S_IRGRP | S_IROTH,
			ddir, NULL, &jiffies_fops)) == NULL)
	{
		pr_err("Cannot create jiffies file\n");
		goto remove_debugfs;
	}
	memset(foo_data, 0, PAGE_SIZE);
   	debugfs_create_file_size("foo",
			S_IRWXU | S_IRGRP | S_IROTH,
			ddir, foo_data, &foo_fops, PAGE_SIZE); 
	pr_info("Debugfs stuff module loaded !\n");
	return 0;
remove_debugfs:
	debugfs_remove_recursive(ddir);
	return -1;
}

static void __exit debugfs_stuff_cleanup(void)
{
	debugfs_remove_recursive(ddir);
	pr_info("Cleaning up debugfs stuff module.\n");
}


module_init(debugfs_stuff_init);
module_exit(debugfs_stuff_cleanup);
