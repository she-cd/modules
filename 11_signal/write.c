#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
int main(int argc, char *argv[])
{
	int fd;//���� int ���͵��ļ�������
	char str1[10] = {0};//�����ȡ������ str1
	char buf1[32] = {0};
	char buf2[32] ="hello";
	fd = open(argv[1],O_RDWR);//���� open ������������ĵ�һ�������ļ���Ȩ��Ϊ�ɶ���д
	if(fd < 0 ){
	printf("file open failed \n");
	return -1;
	}
	
	printf("write before \n");
	write(fd,buf2,sizeof(buf2)); //��/dev/test �ļ�д������
	printf("write after\n");
	close(fd);
	return 0;
}

