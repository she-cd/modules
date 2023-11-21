#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#define CMD_TEST0 _IO( 'L', 0)
#define CMD_TEST1 _IO( 'A', 1)
#define CMD_TEST2 _IOW('L', 2, int)
#define CMD_TEST3 _IOR('L', 3, int)
/*
第四个分区：30-31  代表读写的方向。
00：表示用户程序和驱动程序没有数据传递
10：表示用户程序从驱动里面读数据
01：表示用户程序向驱动里面写数据
11：先写数据到驱动里面然后在从驱动里面把数据读出来。
*/
int main(int argc, char *argv[])
{
	printf("30-31 is %d \n", _IOC_DIR(CMD_TEST0));
	printf("30-31 is %d \n", _IOC_DIR(CMD_TEST3));

	printf("8-15 is %c \n", _IOC_TYPE(CMD_TEST0));
	printf("8-15 is %c \n", _IOC_TYPE(CMD_TEST1));

	printf("0-7 is %d \n", _IOC_NR(CMD_TEST2));
}
