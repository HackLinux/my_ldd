/*
/sys/devices/platform/x86_led/leds/led
或
/sys/class/leds/led
用echo设置brightness文件的值

max_brightness
*/
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>         /**< kmalloc... */
#include <linux/platform_device.h>
#include <linux/leds.h>

#define X86_LEDF_ACTLOW	(1<<0)		/* LED is on when GPIO low */
#define X86_LEDF_TRISTATE	(1<<1)		/* tristate to turn off */
#define X86_GPIO 0

struct simple_led_platdata {
	unsigned int		 gpio;
	unsigned int		 flags;

	char			*name;
	char			*def_trigger;
};


// device
//name字段：
// /sys/devices/platform/x86_led/leds/led
static struct simple_led_platdata x86_pdata_led = {
	.gpio		= X86_GPIO,
	.flags		= X86_LEDF_ACTLOW | X86_LEDF_TRISTATE,
	.name		= "led",
	.def_trigger	= "timer",
};

// 避免删掉模块时出现警告
static void led_dev_release(struct device* dev)
{
    //printk(KERN_NOTICE "do %s case of: Device xxx does not have a release() function, it is broken and must be fixed.\n", __func__);
    return;
}

// 在/sys/devices/platform/生成目录：x86_led
struct platform_device led_device = {
    .name    = "x86_led",
    .id      = -1,  // 注：如为1，则生成目录：x86_led.1
    .dev     = {
        .platform_data = &x86_pdata_led,
        .release = &led_dev_release,
    },
};

//////////////////////////////////////////////////////////////////////////////
/* our context */

struct simple_gpio_led {
	struct led_classdev		 cdev;
	struct simple_led_platdata	*pdata;
};

static inline struct simple_gpio_led *pdev_to_gpio(struct platform_device *dev)
{
	return platform_get_drvdata(dev);
}

static inline struct simple_gpio_led *to_gpio(struct led_classdev *led_cdev)
{
	return container_of(led_cdev, struct simple_gpio_led, cdev);
}

static void simple_led_set(struct led_classdev *led_cdev,
			    enum led_brightness value)
{
	struct simple_gpio_led *led = to_gpio(led_cdev);
	struct simple_led_platdata *pd = led->pdata;

    printk(KERN_NOTICE "[debug] [%s] set led value: %d\n", pd->name, value);
    
}

static int led_remove(struct platform_device *dev)
{
	struct simple_gpio_led *led = pdev_to_gpio(dev);

	led_classdev_unregister(&led->cdev);
	kfree(led);

	return 0;
}

static int led_probe(struct platform_device *dev)
{
	struct simple_led_platdata *pdata = dev->dev.platform_data;
	struct simple_gpio_led *led;
	int ret;

	led = kzalloc(sizeof(struct simple_gpio_led), GFP_KERNEL);
	if (led == NULL) {
		dev_err(&dev->dev, "No memory for device\n");
		return -ENOMEM;
	}

	platform_set_drvdata(dev, led);

	led->cdev.brightness_set = simple_led_set;
	led->cdev.default_trigger = pdata->def_trigger;
	led->cdev.name = pdata->name;
	led->cdev.flags |= LED_CORE_SUSPENDRESUME;

	led->pdata = pdata;

	/* register our new led device */
	ret = led_classdev_register(&dev->dev, &led->cdev);
	if (ret < 0) {
		dev_err(&dev->dev, "led_classdev_register failed\n");
		kfree(led);
		return ret;
	}

	return 0;
}

// driver
static struct platform_driver led_driver = {
    .probe        = led_probe,
    .remove        = led_remove,
    .driver        = {
        .name        = "x86_led",
        .owner        = THIS_MODULE,
    },
};

static int __init led_drv_init(void)
{
    int ret = 0;

    // 先注册设备(适用于静态定义设备结构体)
    ret = platform_device_register(&led_device);
    if (ret)
    {
        printk(KERN_NOTICE "platform_device_register failed!\n");
        return ret;
    }
    // 再注册驱动
    ret = platform_driver_register(&led_driver);
    if (ret)
    {
        printk(KERN_NOTICE "platform_driver_register failed!\n");
        return ret;
    }

    return ret;
}

static void __exit led_drv_exit(void)
{
    // 先卸载驱动
    platform_driver_unregister(&led_driver);
    // 再卸载设备
    platform_device_unregister(&led_device);
}

module_init(led_drv_init);
module_exit(led_drv_exit);

MODULE_AUTHOR("Late Lee");
MODULE_DESCRIPTION("Simple led driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:led");
