/*************************************************************

这里的设备名称须与/dev一致，与/proc/device可以不一致
设备号可在驱动中指定，也可以在/proc/device中查看
*************************************************************/
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

enum {
    CMD_BUFF1 = 0,
    CMD_BUFF2,
    CMD_CLEAR,
    CMD_SET,
    CMD_TEST,
};

#define device "/dev/foo"

typedef struct foo_t
{
    int id;
    char buffer[16];
} foo_t;

int test_lock()
{
	int fd;
    foo_t foo;

	fd = open(device, O_RDWR);
	if (fd < 0)
	{
		perror("Open device faile!");
		return -1;
	}

    ioctl(fd, CMD_TEST, NULL);

	close(fd);
    return 0;
}

int test_ioctl()
{
	int fd;
	char buf[] = "THis is the voice from www.latelee.org";
	char buf2[128]={0};
    char buf3[128] = {0};
	int len;
	fd = open(device, O_RDWR);
	if (fd < 0)
	{
		perror("Open device faile!");
		return -1;
	}
	len = write(fd, buf, sizeof(buf));
	printf("buf: %s %d\n", buf, len);
	len = read(fd, buf2, 32); // 由此指定读取数据，可大可小，但是驱动只读取这个指定的(大者读实际值)，并返回
	printf("buf2: %s %d\n", buf2, len);

    // 使用buffer2
    ioctl(fd, CMD_BUFF2, NULL);

	len = write(fd, "fxxk the world!", sizeof("fxxk the world!"));
	len = read(fd, buf3, 32);
	printf("buf2: %s %d\n", buf3, len);

    // 填充
    //ioctl(fd, CMD_SET, 'c');
	//len = read(fd, buf2, 32);
	//printf("buf2: %s %d\n", buf2, len);
	close(fd);
    return 0;
}

int main(void)
{
    // test_ioctl();
    test_lock();
	return 0;
}
