#include <linux/module.h>  //ģ������ͷ�ļ�
#include <linux/init.h>    //��ʼ�������ͷ�ļ�
#include <linux/miscdevice.h>  //�����豸�������ͷ�ļ�
#include <linux/fs.h>         //�ļ���������
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


struct file_operations hd_fops={  //�ļ���������
	.owner=THIS_MODULE,
	.open=hdmisc_open,
	.release=hdmisc_release,
	.read=hdmisc_read,
	.write=hdmisc_write,

};


struct miscdevice hd_misc={   //�����豸�ṹ��
	.minor=MISC_DYNAMIC_MINOR, //���豸��
	.name="scd",        		//��dev�������豸�ڵ�
	.fops=&hd_fops,             
};

static int helloworldmisc_init(void){
	int ret;
	ret=misc_register(&hd_misc); //ע�������豸
	if(ret < 0){
	printk("register error!!!\n");
	return 0;
	}
	printk("helloworld!!!\n");
	return 0;
}

static void helloworldmisc_exit(void){

	misc_deregister(&hd_misc);  //ע�������豸����
	printk("bye!!!\n");
}


module_init(helloworldmisc_init); //����ģ�����ں���
module_exit(helloworldmisc_exit); //����ģ��ĳ��ں���
MODULE_LICENSE("GPL"); 

