#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <asm/uaccess.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/miscdevice.h>

static struct i2c_client *eeprom_client;
 
#define MAX_SIZE 255  //EEPROM��С
#define EEPROM_PAGE  16 //ҳ�ֽڴ�С
 
static u8 eeprom_buff[255];
static int rv1126_open(struct inode *inode, struct file *file)
{
	printk("rv1126_open-->ok\n");
	return 0;
}
 
static ssize_t rv1126_read(struct file *file, char __user *buf, size_t size, loff_t *seek)
{	
	unsigned long err;
	//�ж�λ���Ƿ񳬳���Χ
	if(*seek+size>MAX_SIZE)
	{
		size=MAX_SIZE-*seek;
	}
	//��ȡ����
	i2c_smbus_read_i2c_block_data(eeprom_client,*seek,size,eeprom_buff);
	err=copy_to_user(buf,eeprom_buff,size);
	if(err!=0)return -1;
	*seek+=size;
	return size;
}
 
static ssize_t rv1126_write(struct file *file, const char __user *buf, size_t size, loff_t *seek)
{
	size_t write_ok_cnt=0;
	unsigned long err;
	int write_byte=0;
	u8 *write_p=eeprom_buff;
	err=copy_from_user(eeprom_buff,buf,size);
	if(err!=0)return -1;
	//�ж�λ���Ƿ񳬳���Χ
	if(*seek+size>MAX_SIZE)
	{
		size=MAX_SIZE-*seek;
	}
	
	while(1)
	{
		if(size>EEPROM_PAGE)
		{
			write_byte=EEPROM_PAGE;
			size-=EEPROM_PAGE;
		}
		else
		{
			write_byte=size;
		}
		
		//д����
		i2c_smbus_write_i2c_block_data(eeprom_client,*seek,write_byte,write_p);
		*seek+=write_byte;
		write_p+=write_byte;
		write_ok_cnt+=write_byte;  //��¼д�ɹ����ֽ���
		//�ȴ�д���
		msleep(10);
		if(write_byte==size)break; //д���
	}
	return write_ok_cnt;
}
 
/*
filp:���������豸�ļ�file�ṹ��ָ��
off:�������Ķ�λƫ��ֵ(�����ɸ�)
whence:�������Ķ�λ��ʼλ��
���أ�������λ������ļ�����дλ�ã�������λ����Ϊ��ֵ
��λ��ʼλ��
  SEEK_SET:0����ʾ�ļ���ͷ
  SEEK_CUR:1����ʾ��ǰλ��
  SEEK_END:2����ʾ�ļ�β
*/
static loff_t rv1126_llseek(struct file *filp, loff_t offset, int whence)
{
	loff_t newpos = 0;
	switch(whence)
	{
		case SEEK_SET:
			newpos = offset;
			break;
		case SEEK_CUR:
			newpos = filp->f_pos + offset;
			break;
		case SEEK_END:
			if(MAX_SIZE+offset>=MAX_SIZE)
			{
				newpos=MAX_SIZE;
			}
			else
			{
				newpos = MAX_SIZE + offset;
			}
			break;
		default:
			return -EINVAL;//��Ч�Ĳ���
	}
	filp->f_pos = newpos;
	return newpos;
}
 
static int rv1126_release(struct inode *inode, struct file *file)
{
	printk("rv1126_release-->ok\n");
	return 0;
}
 
static struct file_operations fops=
{
	.open=rv1126_open,
	.read=rv1126_read,
	.write=rv1126_write,
	.release=rv1126_release,
	.llseek=rv1126_llseek
};
 
/*
Linux�ں˹�������---�豸��
�豸����һ��unsigned int �ı���--32λ��
�豸��=���豸��+���豸��
*/
static struct miscdevice misc=
{
	.minor = MISC_DYNAMIC_MINOR,  /*���豸����255��ʾ�Զ�����     ���豸�Ź̶�Ϊ10*/
	.name = "rv1126_eeprom",  /*/devĿ¼���ļ�����*/
	.fops = &fops, /*�ļ������ӿ�*/
};
 
 
static int rv1126_probe(struct i2c_client *client, const struct i2c_device_id *device_id)
{
	printk("probe���óɹ�:%#X\n",client->addr);
	eeprom_client=client;
	
	/*1. �����豸��ע�ắ��*/
	misc_register(&misc);
	
	return 0;
}
 
static int rv1126_remove(struct i2c_client *client)
{
	/*2. �����豸��ע������*/
	misc_deregister(&misc);
	printk("remove���óɹ�.\n");
	return 0;
}
 
static struct i2c_device_id id_table[]=
{
	{"eeprom", 0},
	{}
};
MODULE_DEVICE_TABLE(i2c, id_table);

static const struct of_device_id eeprom_of_match[] = {
	{ .compatible = "atmel,at24c256B" },
	{ }
};
MODULE_DEVICE_TABLE(of, eeprom_of_match);

 
static struct i2c_driver drv=
{
	.probe=rv1126_probe,
	.remove=rv1126_remove,
	.driver=
	{
		.name="at24c256B",
		.of_match_table = of_match_ptr(eeprom_of_match),
	},
	.id_table=id_table
};
 
static int __init rv1126_drv_init(void)
{	
	/*ע��IIC������*/
	i2c_add_driver(&drv);
    printk("IIC������: ������װ�ɹ�\n");
    return 0;
}
 
static void __exit rv1126_drv_cleanup(void)
{
	/*ע��IIC������*/
	i2c_del_driver(&drv);
    printk("IIC������: ����ж�سɹ�\n");
}
 
module_init(rv1126_drv_init);    /*�������--��װ������ʱ��ִ��*/
module_exit(rv1126_drv_cleanup); /*��������--ж��������ʱ��ִ��*/
 
MODULE_LICENSE("GPL");  /*����ģ������֤--GPL*/

