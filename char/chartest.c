#include <linux/module.h>  //模块的相关头文件
#include <linux/init.h>    //初始化的相关头文件
#include <linux/fs.h>         //文件操作集合
#include <linux/uaccess.h>   //copy_to_user的头文件
#include <linux/kdev_t.h>	//MKDEV所需要的头文件
#include <linux/cdev.h>		//cdev结构体以及cdev_init等
#include <linux/device.h>	//class_create,device_create
#include <linux/io.h>
#include <linux/kernel.h>

#define GPIO5_DR 0x20AC000
#define GPIO1_DR 0x209C000  //LED2
unsigned int* gpio5_dr;
unsigned int* gpio1_dr;

struct cdev LED_cdev;  //cdev结构体用来表示字符设备
struct class * LEDclass;		//class类
struct device * LEDdevice;  //device 设备结构体

dev_t LED_dev;
ssize_t chartest_read (struct file *file, char __user *ubuf, size_t size, loff_t *loff_t){
	char kbuf[]="hd-scd";
	if(copy_to_user(ubuf, kbuf, strlen(kbuf))!=0){

	printk("chartest_read error\n");
	return 0;
	
	}
	printk("chartest_read\n");
	return 0;
}


ssize_t chartest_write (struct file *file, const char __user *ubuf, size_t size, loff_t *loff_t){
	char kbuf[64]={0};
	if(copy_from_user(kbuf,ubuf,size)!=0){

	printk("chartest_write error\n");
	return 0;

	}
	printk("chartest_write\n");
	printk("chartest_write kbuf is %d",kbuf[0]);

	if(kbuf[0]==1) //传入数据为 1 ，蜂鸣器响
	{
	*gpio1_dr |= (1<<3);
	}
	else if(kbuf[0]==0) //传入数据为 0，蜂鸣器关闭
	*gpio1_dr &= ~(1<<3);
	
	return 0;

}
int chartest_open (struct inode *inode, struct file *file){
	printk("chartest_open\n");
	return 0;

}
int chartest_release (struct inode *inode, struct file *file){
	printk("chartest_release\n");
	return 0;

}


struct file_operations chartest_fops={  //文件操作集合
	.owner=THIS_MODULE,
	.open=chartest_open,
	.release=chartest_release,
	.read=chartest_read,
	.write=chartest_write,

};



static int chartest_init(void){
	int ret;

	//char_dev = MKDEV(160, 30);
	/*************静态注册设备号**********************
	ret=register_chrdev_region(char_dev, 1, "class16");
	if(ret!=0){
	printk("register_chrdev_region error\n");
	return 0;
	}
*/
		/*************动态注册设备号**********************/

	ret=alloc_chrdev_region(&LED_dev, 0, 1,"allocLED");
	if(ret!=0){
	printk("alloc_chrdev_region error\n");
	return 0;
	}
	printk("alloc_chrdev_region ok\n");
	printk("major is %d",MAJOR(LED_dev));
	printk("minor is %d",MINOR(LED_dev));
	


	
	gpio1_dr = ioremap(GPIO1_DR, 4);
	if(gpio1_dr == NULL){
	printk("ioremap is error\n");
	return 0;
	}
	printk("ioremap is ok\n");
	
	//初始化cdev
	LED_cdev.owner=THIS_MODULE;
	cdev_init(&LED_cdev, &chartest_fops);
	//像内核注册设备
	cdev_add(&LED_cdev, LED_dev, 1);
	//创建类，在/sys/class下
	LEDclass=class_create(THIS_MODULE, "LEDclass");
	//自动创建设备，在/dev下生成设备节点
	LEDdevice=device_create(LEDclass, NULL, LED_dev, NULL, "LEDdevice");
	
	printk("chartest_init!!!\n");



	return 0;
}

static void chartest_exit(void){
	iounmap(gpio5_dr);
	unregister_chrdev_region(LED_dev, 1);	//注销设备号
	cdev_del(&LED_cdev);				//删除设备
	device_destroy(LEDclass, LED_dev);		//注销设备
	class_destroy(LEDclass);					//删除类
	printk("bye!!!\n");
}


module_init(chartest_init); //驱动模块的入口函数
module_exit(chartest_exit); //驱动模块的出口函数
MODULE_LICENSE("GPL"); 

