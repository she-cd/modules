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
	fd = open(argv[1],O_RDWR);//���� open ������������ĵ�һ�������ļ���Ȩ��Ϊ�ɶ���д
	if(fd < 0 ){
	printf("file open failed \n");
	return -1;
	}
	
	/*����ڶ�������Ϊ topeet���������������� write ������д�� topeet*/
	if (strcmp(argv[2],"linux") == 0 ){
	write(fd,"linux",10);
	}
	/*����ڶ�������Ϊ itop���������������� write ������д�� itop*/
	else if (strcmp(argv[2],"tzh") == 0 ){
	write(fd,"tzh",10);
	}
	/*
	while(1){

		//printf("hello");

	}
	*/
	close(fd);
	return 0;
}

