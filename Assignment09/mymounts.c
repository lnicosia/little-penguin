#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/nsproxy.h>
#include <linux/mount.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lucas Nicosia <lnicosia@student.42.fr>");
MODULE_DESCRIPTION("Listing mountpoint module.");

struct mnt_namespace {
	atomic_t count;
	struct mount *root;
	struct list_head list;
};

static struct proc_dir_entry *mymounts;

static ssize_t list_mountpoints(struct file *file, char __user *buf,
		size_t len, loff_t *ppos)
{
	//struct vfsmount *root_mnt;
	struct mnt_namespace *ns = current->nsproxy->mnt_ns;
	struct mount *mnt;

	pr_info("Listing mountpoints\n");
	list_for_each_entry(mnt, &ns->list, mnt_list) {
		pr_info("Mountpoint\n");
	}
	return 0;
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

	pr_info("Listing mountpoints module loaded !\n");
	return 0;
}

static void __exit	mymounts_cleanup(void)
{
	proc_remove(mymounts);
	pr_info("Cleaning up listing mountpoints module.\n");
}

module_init(mymounts_init);
module_exit(mymounts_cleanup);
