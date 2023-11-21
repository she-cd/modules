#include <linux/init.h>
#include <linux/module.h>
#include <linux/atomic.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/spinlock.h>
#include <linux/spinlock_types.h>
#include <linux/uaccess.h>
#include <linux/delay.h>
#include <linux/semaphore.h>
#include <linux/mutex.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/poll.h>

#define BUFSIZE 1024	//�������ƫ����Ϊ 1024
static char mem[BUFSIZE] = {0};//�������ݴ洢���� mem

struct device_test{
	dev_t dev_num; //�豸��
	int major ; //���豸��
	int minor ; //���豸��
	char kbuf[32];
	int flag; //��־λ
	struct fasync_struct *fasync;

};
struct device_test dev1;
 //wait_queue_head_t read_wq;


DECLARE_WAIT_QUEUE_HEAD(read_wq);

ssize_t module_read(struct file *file, char __user * ubuf, size_t size, loff_t * off){
	//int ret;
	
	loff_t p = *off;//����ȡ���ݵ�ƫ������ֵ�� loff_t ���ͱ��� p
	int i;
	size_t count = size;
	
	//struct device_test*device_read=(struct device_test*)file->private_data;
		pr_err("THIS IS module_read\n");
	if(p > BUFSIZE){//�����ǰƫ��ֵ�����ƫ�������򷵻ش���
		return -1;
	}
	if(count > BUFSIZE - p){
		count = BUFSIZE - p;//���Ҫ��ȡ��ƫ��ֵ����ʣ��Ŀռ䣬���ȡ�����λ��
	}	

	if(copy_to_user(ubuf,mem+p,count)){//�� mem �е�ֵд�� buf�������ݵ��û��ռ�
		printk("copy_to_user error \n");
		return -1;
	}
	for(i=0;i<20;i++){
		printk("buf[%d] is %c\n",i,mem[i]);//�� mem �е�ֵ��ӡ����
	}
	printk("mem is %s,p is %llu,count is %d\n",mem+p,p,count); 
	*off = *off + count;//����ƫ��ֵ
	return count;

}

ssize_t module_write(struct file *file, const char __user *ubuf, size_t size, loff_t *off){

	loff_t p = *off;//��д�����ݵ�ƫ������ֵ�� loff_t ���ͱ��� p
	size_t count = size;
	if(p > BUFSIZE){//�����ǰƫ��ֵ�����ƫ�������򷵻ش���
	return 0;
	}
	if(count > BUFSIZE - p){
	count = BUFSIZE - p;//���Ҫд���ƫ��ֵ����ʣ��Ŀռ䣬��д�뵽���λ��
	}
	if(copy_from_user(mem+p,ubuf,count)){//�� buf �е�ֵ�����û��ռ䴫�ݵ��ں˿ռ�
	printk("copy_to_user error \n");
	return -1;
	}
	printk("mem is %s,p is %llu\n",mem+p,p);//��ӡд���ֵ
	*off = *off + count;//����ƫ��ֵ
	return count;


}

int module_open(struct inode *node, struct file *file){
	
	file->private_data=&dev1;
	pr_err("THIS IS module_open\n");

	return 0;
}

int module_release(struct inode *node, struct file *file){
	

	pr_err("THIS IS module_release\n");

	
	return 0;
}
unsigned int module_poll (struct file *file, struct poll_table_struct * poll_table){

	struct device_test *poll_dev=(struct device_test *)file->private_data; //����˽������
	//short mask=0;
	unsigned int mask=0;
	poll_wait(file,&read_wq,poll_table); //Ӧ������
	if (poll_dev->flag == 1)
	{
		mask |= POLLIN;
	}
	return mask;


}
int module_fasync (int fd, struct file *file, int on){

	struct device_test*device_fasync=(struct device_test*)file->private_data;
	return fasync_helper(fd, file, on, &device_fasync->fasync);


}
loff_t module_llseek(struct file *file, loff_t offset, int whences){

	loff_t new_offset=0;//���� loff_t ���͵��µ�ƫ��ֵ
	switch(whences)//�� lseek �������ݵ� whence ���������ж�
	{
		case SEEK_SET:
			if(offset < 0){
				return -EINVAL;
				break;
			}
			if(offset > BUFSIZE){
				return -EINVAL;
				break;
			}
			new_offset = offset;//��� whence ����Ϊ SEEK_SET������ƫ��ֵΪ offset
			break;
		case SEEK_CUR:
			if(file->f_pos + offset > BUFSIZE){
				return -EINVAL;
				break;
			}
			if(file->f_pos + offset < 0){
				return -EINVAL;
				break;
			}
			new_offset = file->f_pos + offset;//��� whence ����Ϊ SEEK_CUR������ƫ��ֵΪ file->f_pos +offset��file->f_pos Ϊ��ǰ��ƫ��ֵ
			break;
		case SEEK_END:
			if(file->f_pos + offset < 0){
				return -EINVAL;
				break;
			}
			new_offset = BUFSIZE + offset;//��� whence ����Ϊ SEEK_END������ƫ��ֵΪ BUFSIZE + offset��BUFSIZE Ϊ���ƫ����
			break;
		default:
			break;
	}
	file->f_pos = new_offset;//���� file->f_pos ƫ��ֵ
	return new_offset;

}




struct file_operations fops_test={
	.owner=THIS_MODULE,
	.read=module_read,
	.write=module_write,
	.open=module_open,
	.release=module_release,
	.poll=module_poll,
	.fasync=module_fasync,
	.llseek=module_llseek,
};

struct miscdevice  misc_test={
	.minor=MISC_DYNAMIC_MINOR,
	.name="llseek",
	.fops=&fops_test,

};


static int poll_init(void){

	
	misc_register(&misc_test);
	printk("THIS IS PRINTK\n");

	pr_err("THIS IS ATOMIC_T_INIT\n");
	
	return 0;

}

static void poll_exit(void){
	misc_deregister(&misc_test);

	pr_emerg("THIS IS ATOMIC_T_EXIT\n");
}

module_init(poll_init);
module_exit(poll_exit);
MODULE_LICENSE("GPL");

