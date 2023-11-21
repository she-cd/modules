#include <linux/init.h>
#include <linux/module.h>
#include <linux/atomic.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>

atomic_t num=ATOMIC_INIT(1);

ssize_t module_read(struct file *file, char __user * ubuf, size_t size, loff_t * loff_t){

	pr_err("THIS IS module_read\n");
	return 0;

}
ssize_t module_write(struct file *file, const char __user *ubuf, size_t size, loff_t *loff_t){

	pr_err("THIS IS module_write\n");
	return 0;

}

int module_open(struct inode *node, struct file *file){
	pr_err("THIS IS module_open\n");
	//atomic_t num=ATOMIC_INIT(1);
	if(atomic_read(&num)!=1){
		pr_err("open error:Device conflicts\n");
		return 0;
	}
	//atomic_set(num, 0);
	atomic_inc(&num);
	
	return 0;
}

int module_release(struct inode *node, struct file *file){

	pr_err("THIS IS module_release\n");
	//atomic_set(num, 1);
	atomic_dec(&num);
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
	.name="atomic_test",
	.fops=&fops_test,

};


static int atomic_test_init(void){


	misc_register(&misc_test);
	printk("THIS IS PRINTK\n");

	pr_err("THIS IS ATOMIC_T_INIT\n");
	
	return 0;

}

static void atomic_test_exit(void){

	pr_emerg("THIS IS ATOMIC_T_EXIT\n");
}

module_init(atomic_test_init);
module_exit(atomic_test_exit);
MODULE_LICENSE("GPL");

