#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/nsproxy.h>
#include <linux/mount.h>
#include <linux/list.h>
#include <linux/limits.h>
#include <../fs/mount.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lucas Nicosia <lnicosia@student.42.fr>");
MODULE_DESCRIPTION("Mountpoints listing module.");

struct mounts_output {
	int		padding;
	size_t	total_len;
};

static struct proc_dir_entry *mymounts;

static struct mounts_output get_mount_output(struct mnt_namespace *ns)
{
	struct mount *mnt;
	struct mounts_output res;
	struct path mnt_path;
	size_t len;
	size_t max_devname_len;
	char bpath[1024];
	char *p;

	memset(&res, 0, sizeof res);
	max_devname_len = 0;
	list_for_each_entry(mnt, &ns->list, mnt_list) {
		if (mnt->mnt_id != 1) {
			len = strlen(mnt->mnt_devname);
			if (len > max_devname_len)
				max_devname_len = len;
			mnt_path.mnt = &mnt->mnt;
			mnt_path.dentry = mnt->mnt.mnt_root;
			p = d_path(&mnt_path, bpath, 1024);
			res.total_len += strlen(p);
		}
	}
	if (max_devname_len < INT_MAX - 8)
		res.padding = max_devname_len + 8;
	/* padding + 1 for the \n on every line */
	res.total_len += (res.padding + 1) * (ns->mounts - 1);
	return res;
}

static ssize_t list_mountpoints(struct file *file, char __user *buf,
		size_t count, loff_t *f_pos)
{
	char bpath[1024];
	char *p = NULL;
	struct mnt_namespace *ns = current->nsproxy->mnt_ns;
	struct mounts_output mnt_output;
	struct mount *mnt;
	struct path mnt_path;
	int printf_ret;
	ssize_t ret;
	char *out_buf;

	ret = 0;
	printf_ret = 0;
	memset(bpath, 0, 1024);
	mnt_output = get_mount_output(ns);
	if (!(out_buf = kmalloc(mnt_output.total_len,
					GFP_KERNEL))) {
		pr_err("Could not kmalloc output_buffer\n");
		return -1;
	}
	list_for_each_entry(mnt, &ns->list, mnt_list) {
   		mnt_path.mnt = &mnt->mnt;
		mnt_path.dentry = mnt->mnt.mnt_root;
		p = d_path(&mnt_path, bpath, 1024);
		if (IS_ERR(p)) {
			pr_info("d_path() failed\n");
			goto free_out_buf;
		}
		if (mnt->mnt_id != 1) {
			printf_ret += sprintf(out_buf + printf_ret, "%-*s%s\n",
					mnt_output.padding, mnt->mnt_devname, p);
		}
	}
	ret = simple_read_from_buffer(buf, count, f_pos, out_buf, mnt_output.total_len);
free_out_buf:
	kfree(out_buf);
	return ret;
}

static struct proc_ops mountlist_ops = {
	.proc_read = &list_mountpoints
};

static int __init	mymounts_init(void)
{
	if ((mymounts = proc_create("mymounts", 0444, NULL, &mountlist_ops)) == NULL) {
		pr_err("Could not create /proc file\n");
		return -1;
	}
	pr_info("Mountpoints listing module loaded !\n");
	return 0;
}

static void __exit	mymounts_cleanup(void)
{
	proc_remove(mymounts);
	pr_info("Cleaning up mountpoints listing module.\n");
}

module_init(mymounts_init);
module_exit(mymounts_cleanup);
