#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <poll.h>
#include <signal.h>
#include <string.h>
int main(int argc,char *argv[]){
	int fd;//���� int �����ļ�������
	unsigned int off;//�����дƫ��λ��
	char readbuf[13] = {0};//�����ȡ������ readbuf
	char readbuf1[19] = {0};//�����ȡ������ readbuf1
	fd = open(argv[1],O_RDWR,666);//��/dev/test �豸
	if(fd < 0 ){
		printf("file open error \n");
	}
	write(fd,"hello world",13);//�� fd д������ hello world
	off = lseek(fd,0,SEEK_CUR);//��ȡ��ǰλ�õ�ƫ����
	printf("off is %d\n",off);
	
	off = lseek(fd,0,SEEK_SET);//��ƫ��������Ϊ 0
	printf("off is %d\n",off);
	
	read(fd,readbuf,sizeof(readbuf));//��д������ݶ�ȡ�� readbuf ������
	printf("read is %s\n",readbuf);

	off = lseek(fd,0,SEEK_CUR);//��ȡ��ǰλ�õ�ƫ����
	printf("off is %d\n",off);

	off = lseek(fd,-1,SEEK_CUR);//����ǰλ�õ�ƫ������ǰŲ��һλ
	printf("off is %d\n",off);

	write(fd,"Linux",6);//�� fd д������ Linux
	off = lseek(fd,0,SEEK_CUR);//��ȡ��ǰλ�õ�ƫ����
	printf("off is %d\n",off);

	off = lseek(fd,0,SEEK_SET);//��ƫ��������Ϊ 0
	printf("off is %d\n",off);

	read(fd,readbuf1,sizeof(readbuf1));//��д������ݶ�ȡ�� readbuf1 ������
	printf("read is %s\n",readbuf1);
	off = lseek(fd,0,SEEK_CUR);//��ȡ��ǰλ�õ�ƫ����
	printf("off is %d\n",off);

	close(fd);
	return 0;
}


