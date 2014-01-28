#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>

#define INT_DEBUG _IOWR('L', 16, int)

#define device "/dev/foo"

static int g_fd = -1;

void sig_handler(int sig)
{
    static unsigned long long cnt = 1;

    printf("hello world sig_handler!!!\n");
    //printf("Siginfo: si_signo = %d, si_code = %x, si_band = %x\n", info->si_signo, info->si_code, info->si_band);

    cnt++;
    printf("APP Get async signal, cnt: %d\n", cnt);

    return;
}

void trigger_test(void)
{
    int ret = 0;

    ret = ioctl(g_fd, INT_DEBUG, NULL);
    if (ret)
    {
        printf("IOCTL error\n");
        close(g_fd);
        g_fd = -1;
        exit(1);
    }
}

int main(void)
{
    int ret = 0;
    struct sigaction act;

    // 1、关联驱动
    if (g_fd < 0)
    {
        printf("Open it!!!\n");
        g_fd = open(device, O_RDWR);
    }
    if (g_fd < 0) {
        perror("open");
        exit(1);
    }

    fcntl(g_fd, F_SETOWN, getpid());
    fcntl(g_fd, F_SETFL, fcntl(g_fd, F_GETFL) | FASYNC);

    // 2.注册响应函数
    memset(&act, 0, sizeof(act));
    act.sa_handler = sig_handler;
    //act.sa_sigaction = new_op;
    //act.sa_flags = SA_SIGINFO;
    act.sa_flags = 0;
    //ret = sigaction(SIGIO, &act, NULL);
    sigaction(SIGIO, &act, NULL);
    if (ret < 0)
    {
        printf("Install signal error\n");
        if (g_fd > 0)
            close(g_fd);
        g_fd = -1;
        exit(1);
    }

    printf("Install signal successed\n");

    sleep(1);

    while (1)
    {
        //trigger_test();
        sleep(2);
    }
	return 0;
}
