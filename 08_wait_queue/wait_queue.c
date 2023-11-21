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

struct device_test{
	dev_t dev_num; //设备号
	int major ; //主设备号
	int minor ; //次设备号
	char kbuf[32];
	int flag; //标志位
};
struct device_test dev1;
 //wait_queue_head_t read_wq;


DECLARE_WAIT_QUEUE_HEAD(read_wq);

ssize_t module_read(struct file *file, char __user * ubuf, size_t size, loff_t * loff_t){
	int ret;
	struct device_test*device_read=(struct device_test*)file->private_data;

	pr_err("THIS IS module_read\n");
	wait_event_interruptible(read_wq,device_read->flag);
	ret=copy_to_user(ubuf, device_read->kbuf, strlen(device_read->kbuf));
	if(ret!=0){

		pr_err("copy_read error!!!\n");
		return 0;
	}
	printk(KERN_WARNING"write_kbuf value is %s\n",device_read->kbuf);

	return 0;

}

ssize_t module_write(struct file *file, const char __user *ubuf, size_t size, loff_t *loff_t){

	int ret;
	struct device_test*device_write=(struct device_test*)file->private_data;
	pr_err("THIS IS module_write\n");
	ret=copy_from_user(device_write->kbuf, ubuf, size);
	if(ret!=0){

	pr_err("copy_write error!!!\n");
	return 0;

	}
	printk(KERN_WARNING"write_kbuf value is %s\n",device_write->kbuf);
	device_write->flag=1;
	wake_up_interruptible(&read_wq);
	
	
	return 0;

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



struct file_operations fops_test={
	.owner=THIS_MODULE,
	.read=module_read,
	.write=module_write,
	.open=module_open,
	.release=module_release,

};

struct miscdevice  misc_test={
	.minor=MISC_DYNAMIC_MINOR,
	.name="wait_queue",
	.fops=&fops_test,

};


static int atomic_test_init(void){

	
	misc_register(&misc_test);
	printk("THIS IS PRINTK\n");

	pr_err("THIS IS ATOMIC_T_INIT\n");
	
	return 0;

}

static void atomic_test_exit(void){
	misc_deregister(&misc_test);

	pr_emerg("THIS IS ATOMIC_T_EXIT\n");
}

module_init(atomic_test_init);
module_exit(atomic_test_exit);
MODULE_LICENSE("GPL");

