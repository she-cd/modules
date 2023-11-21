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

struct mutex mutex_test;
int flag=1;
ssize_t module_read(struct file *file, char __user * ubuf, size_t size, loff_t * loff_t){
	char kbuf[64]="READ_TEST";
	int ret;
	ret=copy_to_user(ubuf, kbuf, strlen(kbuf));
	if(ret!=0){

	pr_err("copy_read error!!!\n");
	return 0;

	}

	pr_err("THIS IS module_read\n");
	return 0;

}

ssize_t module_write(struct file *file, const char __user *ubuf, size_t size, loff_t *loff_t){
	char kbuf[64]={};
	int ret;
	static int num=0;
	num++;
	ret=copy_from_user(kbuf, ubuf, size);
	if(ret!=0){

	pr_err("copy_write error!!!\n");
	return 0;

	}
	if(strcmp(kbuf,"linux") == 0 ){//如果传递的 kbuf 是 topeet 就睡眠四秒钟
		ssleep(2);
		printk(KERN_WARNING"kbuf %d value is %s",num,kbuf);
	}
	else if(strcmp(kbuf,"tzh") == 0){//如果传递的 kbuf 是 itop 就睡眠两秒钟
		ssleep(2);
		printk(KERN_WARNING"kbuf %d value is %s",num,kbuf);
	}
	//printk(KERN_WARNING"kbuf %d value is %s",num,kbuf);
	
	pr_err("THIS IS module_write\n");
	return 0;

}

int module_open(struct inode *node, struct file *file){
	
	mutex_lock(&mutex_test);
	pr_err("THIS IS module_open\n");

	return 0;
}

int module_release(struct inode *node, struct file *file){
	mutex_unlock(&mutex_test);

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
	.name="mutex",
	.fops=&fops_test,

};


static int atomic_test_init(void){

	mutex_init(&mutex_test);
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

