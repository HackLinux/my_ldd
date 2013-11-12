
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/debugfs.h>
#include <linux/init.h>
#include <linux/types.h>

#include <linux/swap.h>         /**< new add */


static struct dentry *root_entry, *total_pages_entry;
static u64 total_pages;

extern long vm_total_pages;
static int __init debugfs_test_init(void)
{
    total_pages = (u64)vm_total_pages;
    root_entry = debugfs_create_dir("debugfs-root", NULL);
    if (!root_entry) {
        printk("Fail to create proc dir: debugfs-root\n");
        return 1;
    }

    total_pages_entry = debugfs_create_u64("total_pages", 0644, root_entry, &total_pages);

    return 0;
}

static void __exit debugfs_test_exit(void)
{
    debugfs_remove(total_pages_entry);
    debugfs_remove(root_entry);
}

EXPORT_SYMBOL(vm_total_pages);  /**< new add */

module_init(debugfs_test_init);
module_exit(debugfs_test_exit);
MODULE_LICENSE( "GPL" );
