/**
 * @file   simplechar_dev.c
 * @author Late Lee <latelee@163.com>
 * @date   Tue Nov 12 21:07:03 2013
 * 
 * @brief  
 * 
 * 
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>

#include "simplechar.h"

// device
static struct simplechar_platdata foo_pdata = {
    .name        = "gotohell",
};

// 避免删掉模块时出现警告
static void simplechar_dev_release(struct device* dev)
{
    //printk(KERN_NOTICE "do %s case of: Device xxx does not have a release() function, it is broken and must be fixed.\n", __func__);
    return;
}

// 另一文件要使用到，此处不能为static
// 在/sys/devices/platform/生成目录：simplechar
struct platform_device simplechar_device = {
    .name    = "simplechar",
    .id      = -1,  // 注：如何为1，则生成目录：simplechar.1
    .dev     = {
        .platform_data = &foo_pdata,
        .release = &simplechar_dev_release,
    },
};
