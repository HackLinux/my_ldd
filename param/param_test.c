/**
 * @file   param_test.c
 * @author Late Lee <latelee@163.com> from www.latelee.org
 * @date   Tue Aug 24 08:01:50 2010
 * 
 * @brief  A simple test of param based on "Hello World"
 *
 * @env    emacs+gcc+fc10   
 * @how    make clean; make;
 * @test   eg.insmod param_test.ko times=2 whom="Bruce" param=4,3,2,1
 * @note   can not pass parameter in console like
 *         insmod param_test.ko times=2 whom="Bruce Lee" param=4,3,2,1
 *         why?
 *         error occurred in " whom='Bruce Lee' "
 *         !! found the reason: WHOM in console must smaller than WHOM
 *         in this file.
 *         get the information by using:
 *         # dmesg | tail 
 */

#include <linux/module.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/moduleparam.h>

#ifdef DEBUG
/* KERN_INFO */
#define debug(fmt, ...) printk(KERN_DEBUG "param_test: " fmt, ##__VA_ARGS__)
#else
#define debug(fmt, ...)
#endif

static char* whom = "Late Lee";
static int times = 1;
static int param[] = {1,2,3,4};
static int param_nr = 4;

module_param(times, int, S_IRUGO);
module_param(whom, charp, S_IRUGO);
module_param_array(param, int, &param_nr, S_IRUGO);

static int __init param_init(void)
{
    int i = 0;
    for (i=0; i< times; i++)
        debug("(%d) Hello %s!\n", i, whom);
    for (i=0; i<param_nr; i++)
        debug("param[%d]: %d\n", i, param[i]);

    return 0;
}

static void __exit param_exit(void)
{
    debug("Goodbye world!\n");
}

module_init(param_init);
module_exit(param_exit);

MODULE_LICENSE("GPL");
