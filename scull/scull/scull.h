/**
 * @file   scull.h
 * @author Late Lee <latelee@163.com>
 * @date   Fri Sep 17 22:58:47 2010
 * 
 * @brief  
 * 
 * 
 */

#ifndef _SCULL_H_
#define _SCULL_H_

#include <linux/ioctl.h>        /**< _IOW,etc */

#ifdef SCULL_DEBUG
#ifdef __KERNEL__               /**< kernel */
#define PDEBUG(fmt, args...) printk(KERN_DEBUG "scull:" fmt, ##args)
#else                           /**< user space */
#define PDEBUG(fmt, args...) fprintf(stderr, fmt, ##args)
#endif
#else
#define PDEBUG(fmt, args...) 
#endif

#ifndef SCULL_MAJOR
#define SCULL_MAJOR 0           /**< 0 for dynamic */
#endif

#ifndef SCULL_NR_DEVS
#define SCULL_NR_DEVS 4         /**< scull0 to scull3 */
#endif

#ifndef SCULL_P_NR_DEVS
#define SCULL_P_NR_DEVS 4       /**< scullpipe0 to scullpipe0 */
#endif

#ifndef SCULL_QUANTUM
#define SCULL_QUANTUM 4000
#endif

#ifndef SCULL_QSET
#define SCULL_QSET 1000
#endif

#ifndef SCULL_P_BUFFER
#define SCULL_P_BUFFER 4000
#endif

struct scull_qset
{
    void **data;
    struct scull_qset *next;
};

struct scull_dev
{
    struct scull_qset *data;
    int quantum;
    int qset;
    unsigned long size;
    unsigned int access_key;
    struct semaphore sem;
    struct cdev cdev;
};

#define TYPE(minor) (((minor) >> 4) & 0xf)
#define NUM(minor) ((minor) & 0xf)

/* in scull.c */
extern int scull_major;
extern int scull_nr_devs;
extern int scull_qset;

int scull_trim(struct scull_dev *dev);
ssize_t scull_read(struct file *filp, char __user *buf, size_t count,
                   loff_t *f_pos);
ssize_t scull_write(struct file *filp, const char __user *buf, size_t count,
                    loff_t *f_pos);

#define SCULL_IOC_MAGIC 'k'
#define SCULL_IOCRESET		_IO(SCULL_IOC_MAGIC, 0)

#define SCULL_IOCSQUANTUM	_IOW(SCULL_IOC_MAGIC, 1, int)
#define SCULL_IOCSQSET		_IOW(SCULL_IOC_MAGIC, 2, int)
#define SCULL_IOCTQUANTNUM	_IO(SCULL_IOC_MAGIC, 3)
#define SCULL_IOCTQSET		_IO(SCULL_IOC_MAGIC, 4)
#define SCULL_IOCGQUANTNUM	_IOR(SCULL_IOC_MAGIC, 5, int)
#define SCULL_IOCGQSET		_IOR(SCULL_IOC_MAGIC, 6, int)
#define SCULL_IOCQQUANTNUM	_IO(SCULL_IOC_MAGIC, 7)
#define SCULL_IOCQQSET		_IO(SCULL_IOC_MAGIC, 8)
#define SCULL_IOCXQUANTNUM	_IOWR(SCULL_IOC_MAGIC, 9, int)
#define SCULL_IOCXQSET		_IOWR(SCULL_IOC_MAGIC, 10)
#define SCULL_IOCHQUANTNUM	_IO(SCULL_IOC_MAGIC, 11)
#define SCULL_IOCHQSET		_IO(SCULL_IOC_MAGIC, 12)

#define SCULL_P_IOCTSIZE	_IO(SCULL_IOC_MAGIC, 13)
#define SCULL_P_IOCQSIZE	_IO(SCULL_IOC_MAGIC, 14)

#define SCULL_IOC_MAXNR 14

#endif /* _SCULL_H_ */
