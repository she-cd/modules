#include <linux/module.h>  //模块的相关头文件
#include <linux/init.h>    //初始化的相关头文件
#include <linux/miscdevice.h>  //杂项设备驱动相关头文件
#include <linux/fs.h>         //文件操作集合
#include <linux/uaccess.h>   

ssize_t hdmisc_read (struct file *file, char __user *ubuf, size_t size, loff_t *loff_t){
	char kbuf[]="hd-scd";
	if(copy_to_user(ubuf, kbuf, strlen(kbuf))!=0){

	printk("hdmisc_read error\n");
	return 0;
	
	}


	printk("hdmisc_read\n");
	return 0;
}
ssize_t hdmisc_write (struct file *file, const char __user *ubuf, size_t size, loff_t *loff_t){
	char kbuf[64]={0};
	if(copy_from_user(kbuf,ubuf,size)!=0){

	printk("hdmisc_write error\n");
	return 0;

	}
	printk("hdmisc_write kbuf is %s",kbuf);
	printk("hdmisc_write\n");
	return 0;

}
int hdmisc_open (struct inode *inode, struct file *file){
	printk("hdmisc_open\n");
	return 0;

}
int hdmisc_release (struct inode *inode, struct file *file){
	printk("hdmisc_release\n");
	return 0;

}


struct file_operations hd_fops={  //文件操作集合
	.owner=THIS_MODULE,
	.open=hdmisc_open,
	.release=hdmisc_release,
	.read=hdmisc_read,
	.write=hdmisc_write,

};


struct miscdevice hd_misc={   //杂项设备结构体
	.minor=MISC_DYNAMIC_MINOR, //次设备号
	.name="scd",        		//在dev下生成设备节点
	.fops=&hd_fops,             
};

static int helloworldmisc_init(void){
	int ret;
	ret=misc_register(&hd_misc); //注册杂项设备
	if(ret < 0){
	printk("register error!!!\n");
	return 0;
	}
	printk("helloworld!!!\n");
	return 0;
}

static void helloworldmisc_exit(void){

	misc_deregister(&hd_misc);  //注销杂项设备驱动
	printk("bye!!!\n");
}


module_init(helloworldmisc_init); //驱动模块的入口函数
module_exit(helloworldmisc_exit); //驱动模块的出口函数
MODULE_LICENSE("GPL"); 

