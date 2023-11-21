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
���ĸ�������30-31  �����д�ķ���
00����ʾ�û��������������û�����ݴ���
10����ʾ�û�������������������
01����ʾ�û���������������д����
11����д���ݵ���������Ȼ���ڴ�������������ݶ�������
*/
int main(int argc, char *argv[])
{
	printf("30-31 is %d \n", _IOC_DIR(CMD_TEST0));
	printf("30-31 is %d \n", _IOC_DIR(CMD_TEST3));

	printf("8-15 is %c \n", _IOC_TYPE(CMD_TEST0));
	printf("8-15 is %c \n", _IOC_TYPE(CMD_TEST1));

	printf("0-7 is %d \n", _IOC_NR(CMD_TEST2));
}
