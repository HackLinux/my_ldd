/**
 * @file   simplechar_drv.c
 * @author Late Lee <latelee@163.com>
 * @date   Tue Nov 12 21:07:15 2013
 * 
 * @brief  platform模型示例
 * 
 * @note   仅用于insmod和rmmod的测试，不具备字符设备特征
 */
/**


*/
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/platform_device.h>

#include "simplechar.h"

// device
extern struct platform_device simplechar_device;

// our own data
struct simplechar {
    int id;
    struct simplechar_platdata    *pdata;
    char buffer[16];
};


static inline struct simplechar *pdev_to_owndata(struct platform_device *dev)
{
    return platform_get_drvdata(dev);
}

static int simplechar_remove(struct platform_device *dev)
{
	struct simplechar *foo = pdev_to_owndata(dev);

    // 释放自定义数据空间
	kfree(foo);

    return 0;
}

static int simplechar_test(struct platform_device *dev)
{
    struct simplechar *foo = NULL;
    
    foo = pdev_to_owndata(dev);
    
    printk(KERN_NOTICE "%s: get id: %d data: %s\n", __func__, foo->id, foo->buffer);
    
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
    simplechar_test(dev);

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
