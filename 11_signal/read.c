#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <poll.h>
#include <signal.h>
#include <string.h>
char buf1[32] = {0};
int fd;

//SIGIO 信号的信号处理函数
static void func(int signum)
{
	read(fd,buf1,32);
	printf ("buf is %s\n",buf1);
}


int main(int argc, char *argv[])
{
	//定义 int 类型的文件描述符
	char str1[10] = {0};//定义读取缓冲区 str1
	int flags;
	char buf2[32] = "nihao";

	int ret;
	fd = open(argv[1],O_RDWR);//调用 open 函数，打开输入的第一个参数文件，权限为可读可写
	if(fd < 0 ){
		printf("file open failed \n");
		return -1;
	}
	
	
	signal(SIGIO,func);
	//步骤一：使用 signal 函数注册 SIGIO 信号的信号处理函数
	//步骤二：设置能接收这个信号的进程
	//fcntl 函数用来操作文件描述符，
	//F_SETOWN 设置当前接收的 SIGIO 的进程 ID
	fcntl(fd,F_SETOWN,getpid());
	flags = fcntl(fd,F_GETFD); //获取文件描述符标志
	//步骤三 开启信号驱动 IO 使用 fcntl 函数的 F_SETFL 命令打开 FASYNC 标志
	fcntl(fd,F_SETFL,flags| FASYNC);
	while(1);
	close(fd); //关闭文件



	return 0;
}

