基于platform device模型字符设备驱动示例

编译模块：
# make

加载模块：
# insmod GotoHell.ko

驱动自动创建/dev/foo节点

测试程序：
# gcc foo-test.c
# ./a.out

写一字符串(XXX)复制到内核中，在内核中添加字符串(buffer1/buffer2: XXX)，再将它复制到用户空间。
添加ioctl命令。