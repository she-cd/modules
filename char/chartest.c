#include <linux/module.h>  //ģ������ͷ�ļ�
#include <linux/init.h>    //��ʼ�������ͷ�ļ�
#include <linux/fs.h>         //�ļ���������
#include <linux/uaccess.h>   //copy_to_user��ͷ�ļ�
#include <linux/kdev_t.h>	//MKDEV����Ҫ��ͷ�ļ�
#include <linux/cdev.h>		//cdev�ṹ���Լ�cdev_init��
#include <linux/device.h>	//class_create,device_create
#include <linux/io.h>
#include <linux/kernel.h>

#define GPIO5_DR 0x20AC000
#define GPIO1_DR 0x209C000  //LED2
unsigned int* gpio5_dr;
unsigned int* gpio1_dr;

struct cdev LED_cdev;  //cdev�ṹ��������ʾ�ַ��豸
struct class * LEDclass;		//class��
struct device * LEDdevice;  //device �豸�ṹ��

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

	if(kbuf[0]==1) //��������Ϊ 1 ����������
	{
	*gpio1_dr |= (1<<3);
	}
	else if(kbuf[0]==0) //��������Ϊ 0���������ر�
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


struct file_operations chartest_fops={  //�ļ���������
	.owner=THIS_MODULE,
	.open=chartest_open,
	.release=chartest_release,
	.read=chartest_read,
	.write=chartest_write,

};



static int chartest_init(void){
	int ret;

	//char_dev = MKDEV(160, 30);
	/*************��̬ע���豸��**********************
	ret=register_chrdev_region(char_dev, 1, "class16");
	if(ret!=0){
	printk("register_chrdev_region error\n");
	return 0;
	}
*/
		/*************��̬ע���豸��**********************/

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
	
	//��ʼ��cdev
	LED_cdev.owner=THIS_MODULE;
	cdev_init(&LED_cdev, &chartest_fops);
	//���ں�ע���豸
	cdev_add(&LED_cdev, LED_dev, 1);
	//�����࣬��/sys/class��
	LEDclass=class_create(THIS_MODULE, "LEDclass");
	//�Զ������豸����/dev�������豸�ڵ�
	LEDdevice=device_create(LEDclass, NULL, LED_dev, NULL, "LEDdevice");
	
	printk("chartest_init!!!\n");



	return 0;
}

static void chartest_exit(void){
	iounmap(gpio5_dr);
	unregister_chrdev_region(LED_dev, 1);	//ע���豸��
	cdev_del(&LED_cdev);				//ɾ���豸
	device_destroy(LEDclass, LED_dev);		//ע���豸
	class_destroy(LEDclass);					//ɾ����
	printk("bye!!!\n");
}


module_init(chartest_init); //����ģ�����ں���
module_exit(chartest_exit); //����ģ��ĳ��ں���
MODULE_LICENSE("GPL"); 

