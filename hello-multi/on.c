
#include <linux/module.h>
//#include <linux/sched.h>
//#include <linux/kernel.h>
#include <linux/init.h>
//#include <on.h>
#include "configs/on.h"
static void __exit hello_exit(void)
{
    /* KERN_ALERT: display on console */
    printk(KERN_ALERT "Goodbye world!\n");
}

//module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
