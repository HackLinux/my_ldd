/**
 * @file   hello.c
 * @author Late Lee <latelee@163.com>
 * @date   Tue Nov 12 20:59:27 2013
 * 
 * @brief  hello world for linux device driver
 * 
 * 
 */

#include <linux/module.h>

#include <linux/init.h>

static int __init hello_init(void)
{
    printk(KERN_WARNING "Hello world!\n");
    return 0;
}

static void __exit hello_exit(void)
{
    /* KERN_ALERT: display on console */
    printk(KERN_ALERT "Goodbye world!\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Late Lee");
