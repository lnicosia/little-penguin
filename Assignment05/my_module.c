// SPDX-License-Identifier: GPL

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lucas Nicosia <lnicosia@student.42.fr>");
MODULE_DESCRIPTION("Misc char device driver that return my student login.");

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

static const struct file_operations fops = {
	.owner = THIS_MODULE,
	.write = write_student_login,
	.read = read_student_login
};

static struct miscdevice my_misc = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "fortytwo",
	.fops = &fops
};

static int __init student_login_init(void)
{
	if (misc_register(&my_misc)) {
		pr_err("Cannot register misc device\n");
		return -1;
	}
	pr_info("Misc char device driver loaded !\n");
	return 0;
}

static void __exit student_login_cleanup(void)
{
	misc_deregister(&my_misc);
	pr_info("Cleaning up misc char device module.\n");
}


module_init(student_login_init);
module_exit(student_login_cleanup);
