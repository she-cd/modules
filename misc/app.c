#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
int main(int argc,char *argv[])
{
	int fd;
	char buf[64] = {0};
	char writebuf[64]="123456";
	fd = open("/dev/scd",O_RDWR);//���豸�ڵ�
	if(fd < 0)
	{
	perror("open error \n");
	return fd;
	}
	read(fd,buf,sizeof(buf));//���ں˲�����
	printf("read buf is %s\n",buf);

	write(fd,writebuf,sizeof(writebuf));
	close(fd);
	return 0;
}

