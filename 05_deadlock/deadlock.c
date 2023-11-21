#include <linux/init.h>
#include <linux/module.h>
#include <linux/atomic.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/spinlock.h>
#include <linux/spinlock_types.h>
static spinlock_t spinlock_test;
int flag=1;
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
	spin_lock(&spinlock_test);


	return 0;
}

int module_release(struct inode *node, struct file *file){

	pr_err("THIS IS module_release\n");

	spin_unlock(&spinlock_test);
	
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
	.name="deadlock",
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

