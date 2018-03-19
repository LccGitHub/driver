#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>        //man 2 open 查看头文件有哪些
#define FILE	"/dev/chardev" // 刚才mknod创建的设备文件名 双引号不要漏
int main(void)
{
    int fd = -1;
    fd = open(FILE, O_RDWR);
    if (fd < 0){
        printf("open %s error.\n", FILE);
        return -1;
    }
    printf("open %s , fd=%d,success..\n", FILE, fd);
    char buf[1024] = {0};
    // 读写文件
    write(fd,"helloworld",10);
    sleep(1);
    int res = read(fd,buf,100);//最后一个参数为要读取的字节数

    printf("read result：%s, res=%d \n",buf, res);
    // 关闭文件
    close(fd);
    return 0;
}
