#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#define CMD_TEST0 _IO('L',0)
#define CMD_TEST1 _IO('A',1)
#define CMD_TEST2 _IOW('L',2,int)
#define CMD_TEST3 _IOW('L',3,int)
//#define CMD_TEST3 _IOR('L',3,int)
int main(int argc,char *argv[])
{
	int fd;
	char buf[64] ={0};
	fd = open("/dev/hello_misc",O_RDWR);
	if(fd < 0)
	{
		perror("open error \n");
		return fd;
	}
	while(1)
	{
		ioctl(fd,CMD_TEST2,0);
		sleep(2);
		ioctl(fd,CMD_TEST3,1);
	}
	return 0;
}