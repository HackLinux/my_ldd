/**
 * @file   simplechar_drv.c
 * @author Late Lee <latelee@163.com>
 * @date   Tue Nov 12 22:21:19 2013
 * 
 * @brief  platform模型示例
 * 
 * @note   基于platform模型的字符设备驱动示例，添加自动创建设备文件功能。此部分代码可在init时做，也可在probe中做。
 */

#include <linux/module.h>
#include <linux/kernel.h>       /**< printk() */
#include <linux/init.h>
#include <linux/slab.h>         /**< kmalloc... */
#include <linux/platform_device.h>

#include <linux/cdev.h>         /**< cdev_* */
#include <linux/fs.h>
#include <asm/uaccess.h>        /**< copy_*_user */

#include <linux/types.h>        /**< size_t */
#include <linux/errno.h>        /**< error codes */
#include <linux/string.h>

#include <linux/spinlock.h>
#include <linux/delay.h>

#include "simplechar.h"

// device
extern struct platform_device simplechar_device;

// our own data
struct simplechar {
    int id;
    struct simplechar_platdata    *pdata;
    char buffer[16];
};

#define DEBUG

#ifdef DEBUG
/* KERN_INFO */
#define debug(fmt, ...) printk(KERN_NOTICE fmt, ##__VA_ARGS__)
#else
#define debug(fmt, ...)
#endif

#define DEV_NAME "foo"

// 自动创建设备号
//#define HAVE_MAJOR
#ifdef HAVE_MAJOR
#define DEVICE_MAJOR 248
#else
#define DEVICE_MAJOR 0 
#endif

#define FOO_NR_DEVS 1

enum {
    CMD_BUFF1 = 0,
    CMD_BUFF2,
    CMD_CLEAR,
    CMD_SET,
    CMD_TEST,
};

struct cdev foo_cdev;
int foo_major = DEVICE_MAJOR;
int foo_minor = 0;
int foo_nr_devs = FOO_NR_DEVS;
dev_t foo_devno;
struct class* foo_class;

// 保留数量
#define NUM_RES 0
#define NUM_PREFIX 9
#define g_size (1024*64 + NUM_RES)
#define g_real_size (1024*64  - NUM_RES)
// 设置2个缓冲区，可设置读取哪一个
char g_buffer1[g_size] = {0};
char g_buffer2[g_size] = {0};
char* g_ptr = NULL;
char* g_tmp = NULL;

spinlock_t test_lock;

static int open_cnt = 0;

static int foo_open(struct inode *inode, struct file *filp)
{
	debug("in %s()\n", __func__);
    debug("%s() inode: 0x%08x\n", __func__, inode->i_rdev);
    /////////////
    memset(g_buffer1, '\0', g_size);
    memset(g_buffer2, '\0', g_size);
    strcpy(g_buffer1, "buffer1: ");
    strcpy(g_buffer2, "buffer2: ");
    g_ptr = g_buffer1;  // 默认指向第一个缓冲区
    g_tmp = g_ptr + NUM_PREFIX;
    
    open_cnt++;
    printk("open %d\n", open_cnt);
	return 0;
}

// close时调用此函数
static int foo_release(struct inode *inode, struct file *filp)
{
	debug("in %s()\n", __func__);

	return 0;
}

static ssize_t foo_read(struct file *filp, char *buf, size_t count, loff_t *f_ops)
{
	int len;
	char *tmp;
    int ret = 0;
	
	if (count < 0)
		return -EINVAL;
    else if (count > g_real_size)
    {
        printk(KERN_ERR "read count can't be bigger than %d\n", g_real_size);
        return -EINVAL;
    }

	tmp = (char *)kmalloc(sizeof(char) * (g_size+1), GFP_KERNEL);
	sprintf(tmp, "%s", g_ptr);
	len = strlen(tmp);
	if (len < count)
		count = len;
	if ( copy_to_user(buf, tmp, count) )
		return -EFAULT;

    *f_ops += count;
    ret = count;

    //debug("f_pos: %d\n", filp->f_pos);    // 递增
    //debug("in %s() buffer: %s\n", __func__, g_buffer1);
	//debug("in %s() tmp: %s\n", __func__, tmp);
	//debug("in %s() buf: %s\n", __func__, buf);
	//debug("in %s() data: %s\n", __func__, g_ptr);
	kfree(tmp);

	return ret;
}

static ssize_t foo_write(struct file *filp, const char *buf, size_t count, loff_t *f_ops)
{
    int ret = 0;
	if (count < 0)
		return -EINVAL;
    else if (count > g_real_size)
    {
        printk(KERN_ERR "write count can't be bigger than %d\n", g_real_size);
        return -EINVAL;
    }

	if (copy_from_user(g_tmp, buf, count+1))
		return -EFAULT;

    *(g_tmp+count) = '\0';
    *f_ops += count;
    ret = count;

    //debug("in %s() buffer: %s\n", __func__, g_buffer1);
	debug("in %s() buff: %s\n", __func__, buf);
	debug("in %s() data: %s\n", __func__, g_tmp);

	return ret;
}

static long foo_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    char tmp = arg;
    int i = 0;

    int sum = 0;
    
	switch (cmd)
	{
    // 多进程测试，加锁不加锁无影响。
    case CMD_TEST:
    {
        static int cnt = 0;
        cnt++;
        //spin_lock(&test_lock);
        for (i = 0; i < 10; i++)
        {
            sum = i+cnt;
            printk("open idx: %d sleep cnt: %d %d\n", open_cnt, cnt, sum);
            mdelay(300);
        }
        //spin_unlock(&test_lock);
    }
        break;
	case CMD_BUFF1:
        g_ptr = g_buffer1;
        g_tmp = g_ptr + NUM_PREFIX;
        break;
	case CMD_BUFF2:
        g_ptr = g_buffer2;
        g_tmp = g_ptr + NUM_PREFIX;
        break;
	case CMD_CLEAR:
        memset(g_buffer1, '\0', g_size);
        memset(g_buffer2, '\0', g_size);
        break;
	case CMD_SET:
        memset(g_buffer1, tmp, g_size);
        memset(g_buffer2, tmp, g_size);
        break;
        
	default:
		return -EINVAL;
	}
	return 0;
}

static struct file_operations foo_fops = {
	.owner   = THIS_MODULE,
	.open    = foo_open,
	.release = foo_release,
	.read    = foo_read,
	.write   = foo_write,
	.unlocked_ioctl   = foo_ioctl,
};

static inline struct simplechar *pdev_to_owndata(struct platform_device *dev)
{
    return platform_get_drvdata(dev);
}

static int simplechar_test(struct platform_device *dev)
{
    struct simplechar *foo = NULL;
    
    foo = pdev_to_owndata(dev);
    
    printk(KERN_NOTICE "%s: get id: %d data: %s\n", __func__, foo->id, foo->buffer);
    
    return 0;
}

static int simplechar_remove(struct platform_device *dev)
{
	struct simplechar *foo = pdev_to_owndata(dev);

    unregister_chrdev_region(foo_devno, 1);
	cdev_del(&foo_cdev);

    device_destroy(foo_class, MKDEV(foo_major, foo_minor));
    class_destroy(foo_class);

    // 释放自定义数据空间
	kfree(foo);

    printk(KERN_NOTICE "remove...\n");

    return 0;
}

static int simplechar_probe(struct platform_device *dev)
{
    struct simplechar_platdata *pdata = dev->dev.platform_data;
    struct simplechar *foo = NULL;
    int ret = 0;

    printk(KERN_NOTICE "in %s our data name: %s\n", __func__, pdata->name);

    // 申请自定义数据空间
    foo = kzalloc(sizeof(struct simplechar), GFP_KERNEL);
    if (foo == NULL) {
        dev_err(&dev->dev, "No memory for device\n");
        return -ENOMEM;
    }
    // 设置自定义结构体数据
    platform_set_drvdata(dev, foo);

    foo->id = 250;
    strcpy(foo->buffer, "hello world");

    // 简单测试
    //simplechar_test(dev);

    // 字符设备
	cdev_init(&foo_cdev, &foo_fops);
	foo_cdev.owner = THIS_MODULE;
	/* register to who? */
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
		debug(" %s can't get major %d\n", DEV_NAME, foo_major);
		return -EINVAL;
	}
	
	ret = cdev_add(&foo_cdev, foo_devno, 1);
	if (ret < 0)
	{
		debug(" %s cdev_add failure!\n", DEV_NAME);
		return -EINVAL;
	}

    foo_class = class_create(THIS_MODULE, DEV_NAME);
    if (IS_ERR(foo_class))
    {
        printk(KERN_NOTICE "failed to create class.\n");
        return -EINVAL;
    }

    // 自动创建/dev下的节点，名称为DEV_NAME
    device_create(foo_class, NULL, MKDEV(foo_major, foo_minor), NULL, DEV_NAME);

    printk(KERN_NOTICE "register /dev/%s ok: major: %d minor: %d devno: 0x%08x\n", DEV_NAME, foo_major, foo_minor, foo_devno);

    return ret;
}

// driver
static struct platform_driver simplechar_driver = {
    .probe        = simplechar_probe,
    .remove        = simplechar_remove,
    .driver        = {
        .name        = "simplechar",
        .owner        = THIS_MODULE,
    },
};

static int __init simplechar_drv_init(void)
{
    int ret = 0;
    printk(KERN_NOTICE "in %s\n", __func__);

    // 先注册设备(适用于静态定义设备结构体)
    ret = platform_device_register(&simplechar_device);
    if (ret)
    {
        printk(KERN_NOTICE "platform_device_register failed!\n");
        return ret;
    }
    // 再注册驱动
    ret = platform_driver_register(&simplechar_driver);
    if (ret)
    {
        printk(KERN_NOTICE "platform_driver_register failed!\n");
        return ret;
    }

    spin_lock_init(&test_lock);

    printk("%s() ok\n", __func__);
    
    return ret;
}

static void __exit simplechar_drv_exit(void)
{
    printk(KERN_NOTICE "in %s\n", __func__);
    // 先卸载驱动
    platform_driver_unregister(&simplechar_driver);
    // 再卸载设备
    platform_device_unregister(&simplechar_device);
}

module_init(simplechar_drv_init);
module_exit(simplechar_drv_exit);

MODULE_AUTHOR("Late Lee");
MODULE_DESCRIPTION("Simple platform driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:simplechar");
