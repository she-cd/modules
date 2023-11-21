//初始化头文件
#include <linux/init.h>
//最基本的文件，支持动态添加和卸载模块。
#include <linux/module.h>
//包含了 miscdevice 结构的定义及相关的操作函数。
#include <linux/miscdevice.h>
//文件系统头文件，定义文件表结构（file,buffer_head,m_inode 等）
#include <linux/fs.h>
//包含了 copy_to_user、copy_from_user 等内核访问用户进程内存地址的函数定义。
#include <linux/uaccess.h>
//包含了 ioremap、iowrite 等内核访问 IO 内存等函数的定义。
#include <linux/io.h>
//驱动要写入内核，与内核相关的头文件
#include <linux/kernel.h>
#define CMD_TEST1 _IO('A', 1)
#define CMD_TEST0 _IO('L', 0)
#define CMD_TEST2 _IOW('L', 2, int)
#define CMD_TEST3 _IOW('L', 3, int)
#define CMD_TEST4 _IOR('L', 4, int)

ssize_t misc_read(struct file *file, char __user *ubuf, size_t size, loff_t *loff_t)
{
	printk("misc_read\n ");
	return 0;
}
ssize_t misc_write(struct file *file, const char __user *ubuf, size_t size, loff_t *loff_t)
{
/*应用程序传入数据到内核空间，然后控制蜂鸣器的逻辑，在此添加*/
// kbuf 保存的是从应用层读取到的数据
	char kbuf[64] = {0};
// copy_from_user 从应用层传递数据给内核层
	if (copy_from_user(kbuf, ubuf, size) != 0)
	{
// copy_from_user 传递失败打印
		printk("copy_from_user error \n ");
		return -1;
	}
//打印传递进内核的数据
	printk("kbuf is %d\n ", kbuf[0]);
	return 0;
}
/*******************************************************************************
* @brief misc_release : 用户空间关闭设备节点时执行此函数
* @param inode : 文件索引
* @param file : 文件
* @return ：成功返回 0
*****************************************************************************/
int misc_release(struct inode *inode, struct file *file)
{
	printk("hello misc_relaease bye bye \n ");
	return 0;
}
/*******************************************************************************
* @brief misc_open : 用户空间打开设备节点时执行此函数
* @param inode : 文件索引
* @param file : 文件
* @return : 成功返回 0
******************************************************************************/
int misc_open(struct inode *inode, struct file *file)
{
	printk("hello misc_open\n ");
	return 0;
}
/*******************************************************************************
* @brief misc_ioctl : 用户空间使用
ioctl(int fd, unsigned long request, ...(void* arg))时
* 自动执行此函数，根据命令执行对应的操作
* @param file : 设备文件
* @param cmd : 用户空间 ioctl 接口命令 request
* @param value : 用户空间的 arg 指针，依赖于接口命令 request
* @return : 成功返回 0
******************************************************************************/
long misc_ioctl(struct file *file, unsigned int cmd, unsigned long value)
{
	int val;
	switch (cmd)//根据命令进行对应的操作
	{
		case CMD_TEST2:
			printk("LED ON \n");
			printk("value is %ld\n", value);
			break;
		case CMD_TEST3:
			printk("LED OFF \n");
			printk("value is %ld\n", value);
			break;
		case CMD_TEST4:
			val = 12;
			if (copy_to_user((int *)value, &val, sizeof(val)) != 0)
			{
				printk("cpoy_to_usr error \n");
				return -1;
			}
			break;
	}
	return 0;
	}
struct file_operations misc_fops = {
	.owner = THIS_MODULE,
	.open = misc_open,
	.release = misc_release,
	.read = misc_read,
	.write = misc_write,
	.unlocked_ioctl = misc_ioctl /* 64 bit system special */};
	struct miscdevice misc_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "hello_misc",
	.fops = &misc_fops,
};
static int misc_init(void)
{
	int ret;
	ret = misc_register(&misc_dev);
	if (ret < 0)
	{
		printk("misc registe is error \n");
	}
	printk("misc registe is succeed \n");
	return 0;
}
static void misc_exit(void)
{
	misc_deregister(&misc_dev);
	printk(" misc gooodbye! \n");
}
module_init(misc_init);
module_exit(misc_exit);
MODULE_LICENSE("GPL");

