/**
 * @file   foo_drv.c
 * @author Late Lee <latelee@163.com>
 * @date   Tue Nov 12 22:21:19 2013
 * 
 * @brief  platform模型示例
 * 
 * @note   基于platform模型的字符设备驱动示例。
 *         添加异步通知实现。参见同目录测试程序。
 *         添加GPIO转中断实现。检测GPIO状态并触发中断函数。
 * @bug    测试中发现，在ARM上卸载再加载时，内核崩溃了，可能是class退出方面的原因。
 */

#include <linux/module.h>
#include <linux/kernel.h>       /**< printk() */
#include <linux/init.h>
#include <linux/platform_device.h>

#include <linux/cdev.h>         /**< cdev_* */
#include <linux/fs.h>
#include <asm/uaccess.h>        /**< copy_*_user */

#include <linux/types.h>        /**< size_t */
#include <linux/errno.h>        /**< error codes */
#include <linux/string.h>

#include <linux/signal.h>       /**< POLL_IN... */
#include <linux/interrupt.h>    /**< irq */
#include <linux/gpio.h>          /**< gpio */

// 避免删掉模块时出现警告
static void foo_dev_release(struct device* dev)
{
//    printk(KERN_NOTICE "do %s case of: Device xxx does not have a release() function, it is broken and must be fixed.\n", __func__);
    return;
}

// platform设备
static struct platform_device foo_device = {
    .name    = "foo",
    .id      = -1,
    .dev     = {
        //.platform_data = &foo_pdata,
        .release = &foo_dev_release,
    },
};

#define DEBUG

#ifdef DEBUG
/* KERN_INFO */
#define debug(fmt, ...) printk(KERN_NOTICE fmt, ##__VA_ARGS__)
#else
#define debug(fmt, ...)
#endif

#define DEV_NAME "foo"

// 指定设备号或自动创建设备号
//#define HAVE_MAJOR
#ifdef HAVE_MAJOR
#define DEVICE_MAJOR 248
#else
#define DEVICE_MAJOR 0 
#endif

#define FOO_NR_DEVS 1

static struct cdev foo_cdev;
static int foo_major = DEVICE_MAJOR;
static int foo_minor = 0;
static int foo_nr_devs = FOO_NR_DEVS;
static dev_t foo_devno;
static struct class* foo_class;


static struct fasync_struct *my_fasync = NULL;

#define INT_DEBUG _IOWR('L', 16, int)
    
static int foo_open(struct inode *inode, struct file *filp)
{
    //debug("in %s()\n", __func__);
    //debug("%s() inode: 0x%08x\n", __func__, inode->i_rdev);

    return 0;
}

// close时调用此函数
static int foo_release(struct inode *inode, struct file *filp)
{
    //debug("in %s()\n", __func__);

    return 0;
}

static ssize_t foo_read(struct file *filp, char *buf, size_t count, loff_t *f_ops)
{
    int ret = 0;

    return ret;
}

static ssize_t foo_write(struct file *filp, const char *buf, size_t count, loff_t *f_ops)
{
    int ret = 0;

    return ret;
}

// 2.6.36以下版本使用下列接口
//static int foo_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
// 2.6.36及以上版本使用下列函数
static long foo_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    int ret = 0;
    switch (cmd)
    {
        case INT_DEBUG:
        if (my_fasync)
        {
            //printk("--ll debug: ioctl fasync....\n");
            kill_fasync(&my_fasync, SIGIO, POLL_IN);
        }
        break;

        default:
        printk("Invalid cmd.\n");
        ret = -EINVAL;
        break;
    }

    return 0;
}

static int foo_fasync(int fd, struct file *file, int mode)
{
	return fasync_helper(fd, file, mode, &my_fasync);
}

static struct file_operations foo_fops = {
    .owner   = THIS_MODULE,
    .open    = foo_open,
    .release = foo_release,
    .read    = foo_read,
    .write   = foo_write,
// 2.6.36以下版本使用下列接口
//    .ioctl   = foo_ioctl,
//2.6.36及以上版本使用下列接口
    .unlocked_ioctl   = foo_ioctl,
    .fasync  = foo_fasync,
};

static irqreturn_t my_irq(int irq, void *dev_id)
{
    static int cnt = 0;
    cnt++;
    printk("in %s count: %d\n", __func__, cnt);

	return IRQ_HANDLED;
}

static int foo_remove(struct platform_device *dev)
{
    unregister_chrdev_region(foo_devno, 1);
    cdev_del(&foo_cdev);

    device_destroy(foo_class, foo_devno);
    class_destroy(foo_class);


    //printk(KERN_NOTICE "remove...\n");

    return 0;
}

static int request_gpio(void)
{
    int gpio_irq_num = 0;
    int ret = 0;
    // 注册中断(恢复参数按键)
#define RST_PIN (32*2 + 21)
    gpio_irq_num = gpio_to_irq(RST_PIN);
    ret = gpio_request(RST_PIN, "default_pin");
    if (ret)
        return ret;
    ret= gpio_export(RST_PIN,false);
    if(ret)
        goto err_free_sp;
    ret = gpio_direction_input(RST_PIN);
    if (ret)
        goto err_free_sp;

    // IRQF_TRIGGER_RISING：按下再起来为上升沿
    // IRQF_TRIGGER_FALLING：按下为下降沿
    ret = request_irq(gpio_irq_num, my_irq,
                    IRQF_TRIGGER_FALLING | IRQF_DISABLED,
                    "rst", NULL);
err_free_sp:
		gpio_free(RST_PIN);

    return ret;
}

static int foo_probe(struct platform_device *dev)
{
    int ret = 0;

    // 初始化，关联文件操作结构体
    cdev_init(&foo_cdev, &foo_fops);
    foo_cdev.owner = THIS_MODULE;

    if (foo_major)
    {
        foo_devno = MKDEV(foo_major, foo_minor);
        ret = register_chrdev_region(foo_devno, foo_nr_devs, DEV_NAME);
    }
    else
    {
        ret = alloc_chrdev_region(&foo_devno, foo_minor, foo_nr_devs, DEV_NAME); /* get devno */
        foo_major = MAJOR(foo_devno); /* get major */
    }
    if (ret < 0)
    {
        dev_err(&foo_device.dev, "can't get major %d\n", foo_major);
        return -EINVAL;
    }
    
    ret = cdev_add(&foo_cdev, foo_devno, 1);
    if (ret < 0)
    {
        dev_err(&foo_device.dev, "cdev_add failure!\n");
        return -EINVAL;
    }

    // 自动创建/dev下的节点，名称为DEV_NAME
    foo_class = class_create(THIS_MODULE, DEV_NAME);
    if (IS_ERR(foo_class))
    {
        dev_err(&foo_device.dev, "failed to create class.\n");
        return -EINVAL;
    }

    device_create(foo_class, NULL, foo_devno, NULL, DEV_NAME);

    request_gpio();

    dev_info(&foo_device.dev, "Register /dev/%s ok: major: %d minor: %d devno: 0x%08x\n", DEV_NAME, foo_major, foo_minor, foo_devno);

    return ret;
}

// driver
static struct platform_driver foo_driver = {
    .probe        = foo_probe,
    .remove        = foo_remove,
    .driver        = {
        .name        = "foo",
        .owner        = THIS_MODULE,
    },
};

static int __init foo_drv_init(void)
{
    int ret = 0;

    // 先注册设备(适用于静态定义设备结构体)
    ret = platform_device_register(&foo_device);
    if (ret)
    {
        dev_err(&foo_device.dev, "platform_device_register failed!\n");
        return ret;
    }
    // 再注册驱动
    ret = platform_driver_register(&foo_driver);
    if (ret)
    {
        dev_err(&foo_device.dev, "platform_driver_register failed!\n");
        return ret;
    }
    
    dev_info(&foo_device.dev, "Init %s OK!\n", DEV_NAME);
    
    return ret;
}

static void __exit foo_drv_exit(void)
{
    // 先卸载驱动
    platform_driver_unregister(&foo_driver);
    // 再卸载设备
    platform_device_unregister(&foo_device);
}

module_init(foo_drv_init);
module_exit(foo_drv_exit);

MODULE_AUTHOR("Late Lee");
MODULE_DESCRIPTION("Simple platform driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:foo");
