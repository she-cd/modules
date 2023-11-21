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

#define BUFSIZE 1024	//设置最大偏移量为 1024
static char mem[BUFSIZE] = {0};//设置数据存储数组 mem

struct device_test{
	dev_t dev_num; //设备号
	int major ; //主设备号
	int minor ; //次设备号
	char kbuf[32];
	int flag; //标志位
	struct fasync_struct *fasync;

};
struct device_test dev1;
 //wait_queue_head_t read_wq;


DECLARE_WAIT_QUEUE_HEAD(read_wq);

ssize_t module_read(struct file *file, char __user * ubuf, size_t size, loff_t * off){
	//int ret;
	
	loff_t p = *off;//将读取数据的偏移量赋值给 loff_t 类型变量 p
	int i;
	size_t count = size;
	
	//struct device_test*device_read=(struct device_test*)file->private_data;
		pr_err("THIS IS module_read\n");
	if(p > BUFSIZE){//如果当前偏移值比最大偏移量大则返回错误
		return -1;
	}
	if(count > BUFSIZE - p){
		count = BUFSIZE - p;//如果要读取的偏移值超出剩余的空间，则读取到最后位置
	}	

	if(copy_to_user(ubuf,mem+p,count)){//将 mem 中的值写入 buf，并传递到用户空间
		printk("copy_to_user error \n");
		return -1;
	}
	for(i=0;i<20;i++){
		printk("buf[%d] is %c\n",i,mem[i]);//将 mem 中的值打印出来
	}
	printk("mem is %s,p is %llu,count is %d\n",mem+p,p,count); 
	*off = *off + count;//更新偏移值
	return count;

}

ssize_t module_write(struct file *file, const char __user *ubuf, size_t size, loff_t *off){

	loff_t p = *off;//将写入数据的偏移量赋值给 loff_t 类型变量 p
	size_t count = size;
	if(p > BUFSIZE){//如果当前偏移值比最大偏移量大则返回错误
	return 0;
	}
	if(count > BUFSIZE - p){
	count = BUFSIZE - p;//如果要写入的偏移值超出剩余的空间，则写入到最后位置
	}
	if(copy_from_user(mem+p,ubuf,count)){//将 buf 中的值，从用户空间传递到内核空间
	printk("copy_to_user error \n");
	return -1;
	}
	printk("mem is %s,p is %llu\n",mem+p,p);//打印写入的值
	*off = *off + count;//更新偏移值
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

	struct device_test *poll_dev=(struct device_test *)file->private_data; //设置私有数据
	//short mask=0;
	unsigned int mask=0;
	poll_wait(file,&read_wq,poll_table); //应用阻塞
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

	loff_t new_offset=0;//定义 loff_t 类型的新的偏移值
	switch(whences)//对 lseek 函数传递的 whence 参数进行判断
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
			new_offset = offset;//如果 whence 参数为 SEEK_SET，则新偏移值为 offset
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
			new_offset = file->f_pos + offset;//如果 whence 参数为 SEEK_CUR，则新偏移值为 file->f_pos +offset，file->f_pos 为当前的偏移值
			break;
		case SEEK_END:
			if(file->f_pos + offset < 0){
				return -EINVAL;
				break;
			}
			new_offset = BUFSIZE + offset;//如果 whence 参数为 SEEK_END，则新偏移值为 BUFSIZE + offset，BUFSIZE 为最大偏移量
			break;
		default:
			break;
	}
	file->f_pos = new_offset;//更新 file->f_pos 偏移值
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

