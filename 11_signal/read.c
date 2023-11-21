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

//SIGIO �źŵ��źŴ�����
static void func(int signum)
{
	read(fd,buf1,32);
	printf ("buf is %s\n",buf1);
}


int main(int argc, char *argv[])
{
	//���� int ���͵��ļ�������
	char str1[10] = {0};//�����ȡ������ str1
	int flags;
	char buf2[32] = "nihao";

	int ret;
	fd = open(argv[1],O_RDWR);//���� open ������������ĵ�һ�������ļ���Ȩ��Ϊ�ɶ���д
	if(fd < 0 ){
		printf("file open failed \n");
		return -1;
	}
	
	
	signal(SIGIO,func);
	//����һ��ʹ�� signal ����ע�� SIGIO �źŵ��źŴ�����
	//������������ܽ�������źŵĽ���
	//fcntl �������������ļ���������
	//F_SETOWN ���õ�ǰ���յ� SIGIO �Ľ��� ID
	fcntl(fd,F_SETOWN,getpid());
	flags = fcntl(fd,F_GETFD); //��ȡ�ļ���������־
	//������ �����ź����� IO ʹ�� fcntl ������ F_SETFL ����� FASYNC ��־
	fcntl(fd,F_SETFL,flags| FASYNC);
	while(1);
	close(fd); //�ر��ļ�



	return 0;
}

