/**
 * @file   f75111_drv.c
 * @author 
 * @date   
 * 
 * @brief  
 * 
 * @note   
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
#include <linux/ctype.h>

#include "f75111.h"

static void f75111_dev_release(struct device* dev)
{
    return;
}

// platform设备
static struct platform_device f75111_device = {
    .name    = "f75111",
    .id      = -1,
    .dev     = {
        //.platform_data = &foo_pdata,
        .release = &f75111_dev_release,
    },
};

#define DEBUG

#ifdef DEBUG
/* KERN_INFO */
#define debug(fmt, ...) printk(KERN_NOTICE fmt, ##__VA_ARGS__)
#else
#define debug(fmt, ...)
#endif

static struct class* f75111_class;

// 实际中默认使能
static int g_wdt_enable = 0;

// 超时时间
static int g_wdt_timeout = 60;

// cat version　调用此函数
static ssize_t version_show(struct device *dev, 
		struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%s %s %s\n", "v1.0", __DATE__, __TIME__);
}

// cat timeout　调用此函数
static ssize_t timeout_show(struct device *dev, 
		struct device_attribute *attr, char *buf)
{
	//printk("set data to user space: %d\n", g_wdt_timeout);
	return sprintf(buf, "%u\n", g_wdt_timeout);
}

// echo 111 > timeout　调用此函数
static ssize_t timeout_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	g_wdt_timeout = (int)simple_strtol(buf, NULL, 10);
	printk("got data from user space: %d\n", g_wdt_timeout);
    if (g_wdt_timeout>0 && g_wdt_timeout <256)
    {
        printk("set timeout.....\n");
        F75111_SetWDTEnable(g_wdt_timeout);
    }
    // else do nothing
	// 一定要返回size，否则会一直执行
	return size;
}

// cat enable　调用此函数
static ssize_t enable_show(struct device *dev, 
		struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%u\n", g_wdt_enable);
}

// echo 1 > enable　调用此函数
static ssize_t enable_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	g_wdt_enable = (int)simple_strtol(buf, NULL, 10);

	printk("got data from user space: %d\n", g_wdt_enable);
    
    if (g_wdt_enable == 0)
    {
        printk("Disable WDT!!!\n");
        F75111_SetWDTDisable();
    }
    else
    {
        printk("Enable WDT!!!\n");
        F75111_SetWDTEnable(g_wdt_timeout);
    }
	// 一定要返回size，否则会一直执行
	return size;
}

// 生成文件位于/sys/devices/platform/f75111/目录
// 生成文件为version，只读
static DEVICE_ATTR(version, 0444, version_show, NULL);
// 生成文件为wdt_timeout，WDT超时时间
static DEVICE_ATTR(wdt_timeout, 0664, timeout_show, timeout_store);

// 生成文件为wdt_enable，WDT超时时间
static DEVICE_ATTR(wdt_enable, 0664, enable_show, enable_store);

static int f75111_remove(struct platform_device *dev)
{
    device_remove_file(&f75111_device.dev, &dev_attr_version);
	device_remove_file(&f75111_device.dev, &dev_attr_wdt_timeout);
	device_remove_file(&f75111_device.dev, &dev_attr_wdt_enable);
    class_destroy(f75111_class);

    //printk(KERN_NOTICE "remove...\n");

    return 0;
}

static int f75111_probe(struct platform_device *dev)
{
    int ret = 0;

    f75111_class = class_create(THIS_MODULE, "f75111");
    if (IS_ERR(f75111_class))
    {
        dev_err(&f75111_device.dev, "failed to create class.\n");
        return PTR_ERR(f75111_class);
    }

	ret = device_create_file(&f75111_device.dev, &dev_attr_version);
	if (ret)
		goto err_out;
	ret = device_create_file(&f75111_device.dev, &dev_attr_wdt_timeout);
	if (ret)
		goto err_out;
	ret = device_create_file(&f75111_device.dev, &dev_attr_wdt_enable);
	if (ret)
		goto err_out;
err_out:
    return ret;
}

// driver
static struct platform_driver f75111_driver = {
    .probe        = f75111_probe,
    .remove        = f75111_remove,
    .driver        = {
        .name        = "f75111",
        .owner        = THIS_MODULE,
    },
};

static int __init f75111_drv_init(void)
{
    int ret = 0;

    printk("built: %s %s\n", __DATE__, __TIME__);

    F75111_Init();
    
    // 暂时不要狗先
#if 0
    F75111_SetWDTEnable(120);
    g_wdt_enable = 1;
#endif
    // 先注册设备(适用于静态定义设备结构体)
    ret = platform_device_register(&f75111_device);
    if (ret)
    {
        dev_err(&f75111_device.dev, "platform_device_register failed!\n");
        return ret;
    }
    // 再注册驱动
    ret = platform_driver_register(&f75111_driver);
    if (ret)
    {
        dev_err(&f75111_device.dev, "platform_driver_register failed!\n");
        return ret;
    }
    dev_info(&f75111_device.dev, "init OK!\n");
    return ret;
}

static void __exit f75111_drv_exit(void)
{
    // 先卸载驱动
    platform_driver_unregister(&f75111_driver);
    // 再卸载设备
    platform_device_unregister(&f75111_device);

	printk("exit OK!\n");
}

module_init(f75111_drv_init);
module_exit(f75111_drv_exit);

MODULE_AUTHOR("Late Lee");
MODULE_DESCRIPTION("Simple platform driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:f75111");
