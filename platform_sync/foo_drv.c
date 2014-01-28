/**
 * @file   foo_drv.c
 * @author Late Lee <latelee@163.com>
 * @date   2014.01.28
 * 
 * @brief  platformģ��ʾ��
 * 
 * @note   ����platformģ�͵��ַ��豸����ʾ����
 *         ����첽֪ͨʵ�֡��μ�ͬĿ¼���Գ���
 *         ���GPIOת�ж�ʵ�֡����GPIO״̬�������жϺ�����
 *         ���ӹ������У���ⰴ������3�뼴֪ͨӦ�ó���
 * @bug    �����з��֣���ARM��ж���ټ���ʱ�������жϣ��ں˱����ˣ�������class�˳������ԭ��
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
#include <linux/gpio.h>         /**< gpio */
#include <linux/delay.h>

// ����ɾ��ģ��ʱ���־���
static void foo_dev_release(struct device* dev)
{
//    printk(KERN_NOTICE "do %s case of: Device xxx does not have a release() function, it is broken and must be fixed.\n", __func__);
    return;
}

// platform�豸
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

// �����õķŵ�һ��
struct foo_device
{
    char* name;
    int foo_major;
    int foo_minor;
    int foo_nr_devs;
    dev_t foo_devno;

    struct cdev foo_cdev;
    struct work_struct detect;
    struct class* foo_class;
    struct fasync_struct* my_fasync;
};

#define DEV_NAME "foo"

// ָ���豸�Ż��Զ������豸��
//#define HAVE_MAJOR
#ifdef HAVE_MAJOR
#define DEVICE_MAJOR 248
#else
#define DEVICE_MAJOR 0 
#endif

#define FOO_NR_DEVS 1

static struct foo_device foo_dev = {
    .name = DEV_NAME,
    .foo_major = DEVICE_MAJOR,
    .foo_minor = 0,
    .foo_nr_devs = FOO_NR_DEVS,
    .foo_devno = 0,
    
    .my_fasync = NULL,
    .foo_class = NULL,
};

#define INT_DEBUG _IOWR('L', 16, int)

#define RST_PIN (32*2 + 21)
    
static int foo_open(struct inode *inode, struct file *filp)
{
    //debug("in %s()\n", __func__);
    //debug("%s() inode: 0x%08x\n", __func__, inode->i_rdev);

    return 0;
}

// closeʱ���ô˺���
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

// 2.6.36���°汾ʹ�����нӿ�
//static int foo_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
// 2.6.36�����ϰ汾ʹ�����к���
static long foo_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    int ret = 0;
    switch (cmd)
    {
        case INT_DEBUG:
        if (foo_dev.my_fasync)
        {
            //printk("--ll debug: ioctl fasync....\n");
            kill_fasync(&foo_dev.my_fasync, SIGIO, POLL_IN);
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
    return fasync_helper(fd, file, mode, &foo_dev.my_fasync);
}

static struct file_operations foo_fops = {
    .owner   = THIS_MODULE,
    .open    = foo_open,
    .release = foo_release,
    .read    = foo_read,
    .write   = foo_write,
// 2.6.36���°汾ʹ�����нӿ�
//    .ioctl   = foo_ioctl,
//2.6.36�����ϰ汾ʹ�����нӿ�
    .unlocked_ioctl   = foo_ioctl,
    .fasync  = foo_fasync,
};

static int foo_remove(struct platform_device *dev)
{
    unregister_chrdev_region(foo_dev.foo_devno, 1);
    cdev_del(&foo_dev.foo_cdev);

    device_destroy(foo_dev.foo_class, foo_dev.foo_devno);
    class_destroy(foo_dev.foo_class);


    //printk(KERN_NOTICE "remove...\n");

    return 0;
}

static irqreturn_t my_irq(int irq, void *dev_id)
{
    static int cnt = 0;
    struct foo_device *foo_dev = dev_id;

    cnt++;
    printk("in %s count: %d\n", __func__, cnt);

    // ����
    schedule_work(&foo_dev->detect);
    return IRQ_HANDLED;
}

static int request_gpio(void)
{
    int gpio_irq_num = 0;
    int ret = 0;
    // ע���ж�(�ָ���������)
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

    // ���Խ����
    // IRQF_TRIGGER_RISING������������Ϊ������
    // IRQF_TRIGGER_FALLING������Ϊ�½���
    ret = request_irq(gpio_irq_num, my_irq,
                    IRQF_TRIGGER_FALLING/*|IRQF_TRIGGER_RISING */|IRQF_DISABLED,
                    "rst", &foo_dev);

    printk("request irq: %d --> %d\n", RST_PIN, gpio_irq_num);
    return ret;
err_free_sp:
        gpio_free(RST_PIN);

    return ret;
}

// ��������
void foo_waiting(struct work_struct *work)
{
    int press_3s = 0;
    int value = -1;
    int i = 150;

    struct foo_device *foo_dev =
        container_of(work, struct foo_device, detect);

    // ����Ϊ0���ſ�Ϊ1
    value = gpio_get_value(RST_PIN);
    printk("--ll debug: gpio value: %d\n", value);
    // ע�������ʱ���Ǻܾ�ȷ��Ҫʵ�ʲ��Թ���
    // �����Լ��3������
    while (i--)
    {
        value = gpio_get_value(RST_PIN);
        if (value != 0)
        {
            printk("--ll debug--- release the key...\n");
            goto out;
        }
        msleep(10);
    }
    printk("==ll debug ==You press it 3 seconds.\n");
    press_3s = 1;

    if (press_3s)
    {
        if (foo_dev->my_fasync)
        {
            kill_fasync(&foo_dev->my_fasync, SIGIO, POLL_IN);
        }
    }

out:
    return;
}

static int foo_probe(struct platform_device *dev)
{
    int ret = 0;

    // ��ʼ���������ļ������ṹ��
    cdev_init(&foo_dev.foo_cdev, &foo_fops);
    foo_dev.foo_cdev.owner = THIS_MODULE;

    if (foo_dev.foo_major)
    {
        foo_dev.foo_devno = MKDEV(foo_dev.foo_major, foo_dev.foo_minor);
        ret = register_chrdev_region(foo_dev.foo_devno, foo_dev.foo_nr_devs, foo_dev.name);
    }
    else
    {
        ret = alloc_chrdev_region(&foo_dev.foo_devno, foo_dev.foo_minor, foo_dev.foo_nr_devs, foo_dev.name); /* get devno */
        foo_dev.foo_major = MAJOR(foo_dev.foo_devno); /* get major */
    }
    if (ret < 0)
    {
        dev_err(&foo_device.dev, "can't get major %d\n", foo_dev.foo_major);
        return -EINVAL;
    }
    
    ret = cdev_add(&foo_dev.foo_cdev, foo_dev.foo_devno, 1);
    if (ret < 0)
    {
        dev_err(&foo_device.dev, "cdev_add failure!\n");
        return -EINVAL;
    }

    // �Զ�����/dev�µĽڵ㣬����ΪDEV_NAME
    foo_dev.foo_class = class_create(THIS_MODULE, foo_dev.name);
    if (IS_ERR(foo_dev.foo_class))
    {
        dev_err(&foo_device.dev, "failed to create class.\n");
        return -EINVAL;
    }

    device_create(foo_dev.foo_class, NULL, foo_dev.foo_devno, NULL, foo_dev.name);

    request_gpio();

    // ��ʼ����������
    INIT_WORK(&foo_dev.detect, foo_waiting);

    dev_info(&foo_device.dev, "Register /dev/%s ok: major: %d minor: %d devno: 0x%08x\n", 
            foo_dev.name, foo_dev.foo_major, foo_dev.foo_minor, foo_dev.foo_devno);

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

    // ��ע���豸(�����ھ�̬�����豸�ṹ��)
    ret = platform_device_register(&foo_device);
    if (ret)
    {
        dev_err(&foo_device.dev, "platform_device_register failed!\n");
        return ret;
    }
    // ��ע������
    ret = platform_driver_register(&foo_driver);
    if (ret)
    {
        dev_err(&foo_device.dev, "platform_driver_register failed!\n");
        return ret;
    }
    
    dev_info(&foo_device.dev, "Init %s OK!\n", foo_dev.name);
    
    return ret;
}

static void __exit foo_drv_exit(void)
{
    // ��ж������
    platform_driver_unregister(&foo_driver);
    // ��ж���豸
    platform_device_unregister(&foo_device);
}

module_init(foo_drv_init);
module_exit(foo_drv_exit);

MODULE_AUTHOR("Late Lee");
MODULE_DESCRIPTION("Simple platform driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:foo");
