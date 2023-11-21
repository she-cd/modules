#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
int main(int argc, char *argv[])
{
	int fd;//定义 int 类型的文件描述符
	char str1[10] = {0};//定义读取缓冲区 str1
	char buf1[32] = {0};
	char buf2[32] ="hello";
	fd = open(argv[1],O_RDWR);//调用 open 函数，打开输入的第一个参数文件，权限为可读可写
	if(fd < 0 ){
	printf("file open failed \n");
	return -1;
	}
	
	printf("write before \n");
	write(fd,buf2,sizeof(buf2)); //向/dev/test 文件写入数据
	printf("write after\n");
	close(fd);
	return 0;
}

