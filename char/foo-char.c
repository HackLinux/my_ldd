/*************************************************************************
简单的字符型设备驱动
从应用层获取一数据，再复制到应用层(在前面添加字符串)。
注册设备号及设备号的几个宏，均系ldd3例子scull。

2011-04-29
log
2013-11-09 late lee
固定2个64KB缓冲区，默认使用缓冲区1，提供ioctl选择缓冲区，填充缓冲区
每个缓冲区使用前缀作区别，并将前缀传递到应用程序
*************************************************************************/

#include <linux/module.h>
#include <linux/kernel.h>       /**< printk() */
#include <linux/init.h>

#include <linux/cdev.h>         /**< cdev_* */
#include <linux/fs.h>
#include <asm/uaccess.h>        /**< copy_*_user */

#include <linux/types.h>        /**< size_t */
#include <linux/errno.h>        /**< error codes */
#include <linux/string.h>

#define DEBUG

#ifdef DEBUG
/* KERN_INFO */
#define debug(fmt, ...) printk(KERN_NOTICE fmt, ##__VA_ARGS__)
#else
#define debug(fmt, ...)
#endif

#define DEV_NAME "foo"

#define HAVE_MAJOR
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
};

struct cdev foo_cdev;
int foo_major = DEVICE_MAJOR;
int foo_minor = 0;
int foo_nr_devs = FOO_NR_DEVS;
dev_t devno;

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

static int foo_open(struct inode *inode, struct file *filp)
{
	debug("in %s()\n", __func__);
    debug("%s() inode: %x\n", __func__, inode->i_rdev);
    /////////////
    memset(g_buffer1, '\0', g_size);
    memset(g_buffer2, '\0', g_size);
    strcpy(g_buffer1, "buffer1: ");
    strcpy(g_buffer2, "buffer2: ");
    g_ptr = g_buffer1;  // 默认指向第一个缓冲区
    g_tmp = g_ptr + NUM_PREFIX;
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

    debug("f_pos: %d", filp->f_pos);    // 递增
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

static int foo_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
    char tmp = arg;
	switch (cmd)
	{
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
	.ioctl   = foo_ioctl,
};

static int __init foo_init(void)
{
	int ret = -1;

	cdev_init(&foo_cdev, &foo_fops);
	foo_cdev.owner = THIS_MODULE;
	/* register to who? */
	if (foo_major)
	{
		devno = MKDEV(foo_major, foo_minor);
		ret = register_chrdev_region(devno, foo_nr_devs, DEV_NAME);
	}
	else
	{
		ret = alloc_chrdev_region(&devno, foo_minor, foo_nr_devs, DEV_NAME); /* get devno */
		foo_major = MAJOR(devno); /* get major */
		
	}
	if (ret < 0)
	{
		debug(" %s can't get major %d\n", DEV_NAME, foo_major);
		return -EINVAL;
	}
	
	ret = cdev_add(&foo_cdev, devno, 1);
	if (ret < 0)
	{
		debug(" %s cdev_add failure!\n", DEV_NAME);
		return -EINVAL;
	}

	debug("%s init ok! devno: %x\n", DEV_NAME, devno);
	return 0;
}

static void __exit foo_exit(void)
{
	unregister_chrdev_region(devno, 1);
	cdev_del(&foo_cdev);
	debug("%s exit ok!\n", DEV_NAME);
}

module_init(foo_init);
module_exit(foo_exit);
MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Chiangchin Li");
