//��ʼ��ͷ�ļ�
#include <linux/init.h>
//��������ļ���֧�ֶ�̬��Ӻ�ж��ģ�顣
#include <linux/module.h>
//������ miscdevice �ṹ�Ķ��弰��صĲ���������
#include <linux/miscdevice.h>
//�ļ�ϵͳͷ�ļ��������ļ���ṹ��file,buffer_head,m_inode �ȣ�
#include <linux/fs.h>
//������ copy_to_user��copy_from_user ���ں˷����û������ڴ��ַ�ĺ������塣
#include <linux/uaccess.h>
//������ ioremap��iowrite ���ں˷��� IO �ڴ�Ⱥ����Ķ��塣
#include <linux/io.h>
//����Ҫд���ںˣ����ں���ص�ͷ�ļ�
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
/*Ӧ�ó��������ݵ��ں˿ռ䣬Ȼ����Ʒ��������߼����ڴ����*/
// kbuf ������Ǵ�Ӧ�ò��ȡ��������
	char kbuf[64] = {0};
// copy_from_user ��Ӧ�ò㴫�����ݸ��ں˲�
	if (copy_from_user(kbuf, ubuf, size) != 0)
	{
// copy_from_user ����ʧ�ܴ�ӡ
		printk("copy_from_user error \n ");
		return -1;
	}
//��ӡ���ݽ��ں˵�����
	printk("kbuf is %d\n ", kbuf[0]);
	return 0;
}
/*******************************************************************************
* @brief misc_release : �û��ռ�ر��豸�ڵ�ʱִ�д˺���
* @param inode : �ļ�����
* @param file : �ļ�
* @return ���ɹ����� 0
*****************************************************************************/
int misc_release(struct inode *inode, struct file *file)
{
	printk("hello misc_relaease bye bye \n ");
	return 0;
}
/*******************************************************************************
* @brief misc_open : �û��ռ���豸�ڵ�ʱִ�д˺���
* @param inode : �ļ�����
* @param file : �ļ�
* @return : �ɹ����� 0
******************************************************************************/
int misc_open(struct inode *inode, struct file *file)
{
	printk("hello misc_open\n ");
	return 0;
}
/*******************************************************************************
* @brief misc_ioctl : �û��ռ�ʹ��
ioctl(int fd, unsigned long request, ...(void* arg))ʱ
* �Զ�ִ�д˺�������������ִ�ж�Ӧ�Ĳ���
* @param file : �豸�ļ�
* @param cmd : �û��ռ� ioctl �ӿ����� request
* @param value : �û��ռ�� arg ָ�룬�����ڽӿ����� request
* @return : �ɹ����� 0
******************************************************************************/
long misc_ioctl(struct file *file, unsigned int cmd, unsigned long value)
{
	int val;
	switch (cmd)//����������ж�Ӧ�Ĳ���
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

