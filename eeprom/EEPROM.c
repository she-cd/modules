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
 
#define MAX_SIZE 255  //EEPROM大小
#define EEPROM_PAGE  16 //页字节大小
 
static u8 eeprom_buff[255];
static int rv1126_open(struct inode *inode, struct file *file)
{
	printk("rv1126_open-->ok\n");
	return 0;
}
 
static ssize_t rv1126_read(struct file *file, char __user *buf, size_t size, loff_t *seek)
{	
	unsigned long err;
	//判断位置是否超出范围
	if(*seek+size>MAX_SIZE)
	{
		size=MAX_SIZE-*seek;
	}
	//读取数据
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
	//判断位置是否超出范围
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
		
		//写数据
		i2c_smbus_write_i2c_block_data(eeprom_client,*seek,write_byte,write_p);
		*seek+=write_byte;
		write_p+=write_byte;
		write_ok_cnt+=write_byte;  //记录写成功的字节数
		//等待写完成
		msleep(10);
		if(write_byte==size)break; //写完毕
	}
	return write_ok_cnt;
}
 
/*
filp:待操作的设备文件file结构体指针
off:待操作的定位偏移值(可正可负)
whence:待操作的定位起始位置
返回：返回移位后的新文件读、写位置，并且新位置总为正值
定位起始位置
  SEEK_SET:0，表示文件开头
  SEEK_CUR:1，表示当前位置
  SEEK_END:2，表示文件尾
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
			return -EINVAL;//无效的参数
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
Linux内核管理驱动---设备号
设备号是一个unsigned int 的变量--32位。
设备号=主设备号+次设备号
*/
static struct miscdevice misc=
{
	.minor = MISC_DYNAMIC_MINOR,  /*次设备号填255表示自动分配     主设备号固定为10*/
	.name = "rv1126_eeprom",  /*/dev目录下文件名称*/
	.fops = &fops, /*文件操作接口*/
};
 
 
static int rv1126_probe(struct i2c_client *client, const struct i2c_device_id *device_id)
{
	printk("probe调用成功:%#X\n",client->addr);
	eeprom_client=client;
	
	/*1. 杂项设备的注册函数*/
	misc_register(&misc);
	
	return 0;
}
 
static int rv1126_remove(struct i2c_client *client)
{
	/*2. 杂项设备的注销函数*/
	misc_deregister(&misc);
	printk("remove调用成功.\n");
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
	/*注册IIC驱动端*/
	i2c_add_driver(&drv);
    printk("IIC驱动端: 驱动安装成功\n");
    return 0;
}
 
static void __exit rv1126_drv_cleanup(void)
{
	/*注销IIC驱动端*/
	i2c_del_driver(&drv);
    printk("IIC驱动端: 驱动卸载成功\n");
}
 
module_init(rv1126_drv_init);    /*驱动入口--安装驱动的时候执行*/
module_exit(rv1126_drv_cleanup); /*驱动出口--卸载驱动的时候执行*/
 
MODULE_LICENSE("GPL");  /*设置模块的许可证--GPL*/

