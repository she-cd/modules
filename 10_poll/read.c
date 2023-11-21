#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <poll.h>

int main(int argc, char *argv[])
{
	int fd;//���� int ���͵��ļ�������
	char str1[10] = {0};//�����ȡ������ str1
	char buf1[32] = {0};
	char buf2[32] = "nihao";
	struct pollfd fds[1];
	int ret;
	fd = open(argv[1],O_RDWR);//���� open ������������ĵ�һ�������ļ���Ȩ��Ϊ�ɶ���д
	if(fd < 0 ){
		printf("file open failed \n");
		return -1;
	}
	printf("read before \n");


	fds[0] .fd =fd;
	fds[0].events = POLLIN; //���������Ƿ���Զ�ȡ
	printf("read before \n");
	while (1)
	{
		ret = poll(fds,1,3000); //��ѯ�ļ��Ƿ�ɲ�������ʱ 3000ms
		if(!ret){ //��ʱ
			printf("time out !!\n,");
		}else if(fds[0].revents == POLLIN) //��������¼��������ݿɶ�ȡ
		{	
			read(fd,buf1,sizeof(buf1)); //��/dev/test �ļ���ȡ����
			printf("buf is %s \n,",buf1); //��ӡ��ȡ������
			sleep(1);
		}
	}
	//printf("buf is %s \n",buf1);
	printf("read after \n");

	close(fd);
	return 0;
}

