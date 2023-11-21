#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc,char *argv[])
{
	int fd;
	char buf[64] = {0};
	char writebuf[64]={0};
	fd = open("/dev/ledtest",O_RDWR);//打开设备节点
	if(fd < 0)
	{
	perror("open error \n");
	return fd;
	}
	read(fd,buf,sizeof(buf));//读内核层数据
	printf("read buf is %s\n",buf);

	writebuf[0] = atoi(argv[1]);
	write(fd,writebuf,sizeof(writebuf));
	close(fd);
	return 0;
}

