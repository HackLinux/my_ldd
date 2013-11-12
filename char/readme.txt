编译模块：
# make

加载模块：
# insmod GotoHell.ko

创建设备节点：
# mknod /dev/foo c 250 0

测试程序：
# gcc foo-test.c
# ./a.out

写一字符串(XXX)复制到内核中，在内核中添加字符串(buffer1/buffer2: XXX)，再将它复制到用户空间。
添加ioctl命令。

问题：
在驱动的read中，分配了count+1字节的空间，当应用层传递较小值(即count，如10)，而驱动返回较大值(即tmp的长度)时，依然能正常读取所有字符串。
以count返回到应用层没此问题。

自动分配设备号的，如何添加设备节点？
-->查看/proc/device的设备号，再创建节点。
cat /proc/device

设备名称：
/dev/下的可以与/proc/device不同，但是应用程序打开的必须是/dev中的