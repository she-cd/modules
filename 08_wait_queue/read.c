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
	char buf2[32] = "nihao";
	fd = open(argv[1],O_RDWR);//���� open ������������ĵ�һ�������ļ���Ȩ��Ϊ�ɶ���д
	if(fd < 0 ){
	printf("file open failed \n");
	return -1;
	}
	printf("read before \n");
	read(fd,buf1,sizeof(buf1)); //��/dev/test �ļ���ȡ����
	printf("buf is %s \n",buf1);
	printf("read after \n");

	close(fd);
	return 0;
}

