/*
 * @Author: topeet
 * @Description: ����ƽ̨�豸ģ�͵�driver.c,��probe�����л�ȡӲ����Դ��ע��һ�������豸
 */
//��ʼ��ͷ�ļ�
#include <linux/init.h>	
//��������ļ���֧�ֶ�̬��Ӻ�ж��ģ�顣		   
#include <linux/module.h>
//ƽ̨�豸����Ҫ��ͷ�ļ�?		  
#include <linux/platform_device.h> 
#include <linux/ioport.h>

//�ļ�ϵͳͷ�ļ��������ļ���ṹ��file,buffer_head,m_inode�ȣ�
#include <linux/fs.h>	
//������copy_to_user��copy_from_user���ں˷����û������ڴ��ַ�ĺ������塣	 
#include <linux/uaccess.h>	 
//������ioremap��iowrite���ں˷���IO�ڴ�Ⱥ����Ķ��塣
#include <linux/io.h>		
#include <linux/kernel.h>
#include <linux/delay.h>
#include <asm/irq.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include<linux/of.h>
#include<linux/of_address.h>



//�Ĵ�����ַ����
#define GPIO5_DR 0x020AC000
//���ӳ����������ַ���׵�ַ
unsigned int *vir_gpio5_dr;


#define FIRST_DEV MKDEV(157, 5)
static struct cdev firstdrv_cdev;
struct device_node * test_device_node;
struct property * test_node_property;
u32 out_values[2]={0};


static int firstdrv_open(struct inode *inode, struct file *file)
{
    printk(KERN_NOTICE "Char Device opened!\n");

    return 0;
}

static ssize_t firstdrv_write(struct file *file, const char __user *userbuf,
         size_t bytes, loff_t *off)
{
    //int val;
   // copy_from_user(&val, userbuf, bytes);					//�����û��ռ����ݵ��ں˿ռ�

	 char kbuf[64] = {0};
    // copy_from_user ��Ӧ�ò㴫�����ݸ��ں˲�
	if(copy_from_user(&kbuf,userbuf,bytes)!= 0) 
	{
        // copy_from_user ����ʧ�ܴ�ӡ
		printk("copy_from_user error \n ");
		return -1;
	}
    //��ӡ���ݽ��ں˵�����
    printk("kbuf is %d\n ",kbuf[0]); 
	if(kbuf[0]==1) //��������Ϊ1 ����������
	{
		*vir_gpio5_dr |= (1<<1);
	}
	else if(kbuf[0]==0) //��������Ϊ0���������ر�
		*vir_gpio5_dr &= ~(1<<1);
	return 0;
    
}

static struct class firstdrv_class = {
    .name        = "test",
};


static struct file_operations firstdrv_ops = {
    .owner = THIS_MODULE,
    .open = firstdrv_open,
    .write = firstdrv_write,
};


/****************************************************************************************
 * @brief beep_probe : ���豸��Ϣ�㣨device.c��ƥ��ɹ����Զ�ִ�д˺�����
 * @param inode : �ļ�����
 * @param file  : �ļ�
 * @return �ɹ����� 0 	    	
 ****************************************************************************************/
int beep_probe(struct platform_device *pdev)
{
	printk("beep_probe\n");
	/*��ȡӲ����Դ����һ�� ���Ƽ�*/
	//printk("beep_res is %s\n",pdev->resource[0].name);
	//return 0;
	/*��ȡӲ����Դ�������� �Ƽ�*/
    
	//beep_mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);	  /* ��ȡplatform_device��resource��Դ */
/*
	if (beep_mem == NULL)
	{
		printk("platform_get_resource is error\n");
		return -EBUSY;
	}
	printk("beep_res start is 0x%x \n", beep_mem->start);
	printk("beep_res end is 0x%x \n", beep_mem->end);
   */
	//ӳ��GPIO��Դ
	/*vir_gpio5_dr = ioremap(beep_mem->start, 4);
	if (vir_gpio5_dr == NULL)
	{
		printk("GPIO5_DR ioremap is error \n");
		return EBUSY;
	}
	printk("GPIO5_DR ioremap is ok \n");
   */
   
	/*********************ͨ��������ȡӲ����Դ**************************/
	test_device_node = of_find_node_by_path("/test-tree-platform");//����豸�ڵ�
	if( test_device_node == NULL){
		printk("of_find_node_by_path is error\n");
		return-1;
	}
    printk("of_find_node_by_path is ok\n");
	
	test_node_property = of_property_read_u32_array(pdev->dev.of_node,"reg",out_values,2);

	if(test_node_property<0){
		printk("of_property_read_u32_array is error\n");
		return-1;
	}
	printk("out_values[0] is 0x%08x\n",out_values[0]);
	printk("out_values[1] is 0x%08x\n",out_values[1]);
	/*********************ӳ�������ַ*************************/
	vir_gpio5_dr=of_iomap(pdev->dev.of_node,0);
     if(vir_gpio5_dr==NULL){
		 printk("of_iomap is error\n");
		 return-1;
	 }



	//ע��char�豸

    int ret;

    ret = register_chrdev_region(FIRST_DEV, 1, "test");
	 if (ret) {
        printk(KERN_ERR "Unable to register firstdrv\n");
        goto err_reg;
    }
    cdev_init(&firstdrv_cdev, &firstdrv_ops);
    ret = cdev_add(&firstdrv_cdev, FIRST_DEV, 1);
    if (ret) {
        printk(KERN_ERR "Unable to add cdev\n");
        goto err_add_cdev;
    }
    class_register(&firstdrv_class);
	device_create(&firstdrv_class, NULL, FIRST_DEV, NULL, "test");
	    return 0;
err_add_cdev:
    cdev_del(&firstdrv_cdev);
err_reg:
    unregister_chrdev_region(FIRST_DEV, 1);
	 return 0;

}

int beep_remove(struct platform_device *pdev)
{
	printk("beep_remove\n");
	return 0;
}

const struct platform_device_id beep_idtable = {
	.name = "test",
};

const struct of_device_id of_match_table_test[]={
	{.compatible="test1111112"},
		{},
};




//?platform?�����ṹ��
struct platform_driver beep_driver = {
	.probe = beep_probe,		/* ̽�⺯�� */
	.remove = beep_remove,
	.driver = {
		.owner = THIS_MODULE,
		.name = "test",
		.of_match_table=of_match_table_test
	},
	.id_table = &beep_idtable,	/* ƥ��������ƥ��ɹ����򲻻��ٱȽ�driver.name��Ա */

};


static int beep_driver_init(void)
{
	int ret = 0;
	//?platform����ע�ᵽ?Linux?�ں�
	ret = platform_driver_register(&beep_driver);
	if (ret < 0)
	{
		printk("platform_driver_register error \n");
	}
	printk("platform_driver_register ok \n");

	return 0;
}
/**
  �豸ģ��ע��
**/
static void beep_driver_exit(void)
{
	platform_driver_unregister(&beep_driver);
	cdev_del(&firstdrv_cdev);
	unregister_chrdev_region(FIRST_DEV, 1);

	class_destroy(&firstdrv_class);
	//iounmap(pdev->dev.of_node);
	printk("gooodbye! \n");
}

module_init(beep_driver_init);	  /* ������ں��� */
module_exit(beep_driver_exit);	  /* �������ں��� */
MODULE_LICENSE("GPL");	  

/* 
  ��2.4.10�汾�ں˿�ʼ��ģ�����ͨ��MODULE_LICENSE������
  ��ģ������֤�������ڼ��ش�ģ��ʱ�����յ��ں˱���Ⱦ
 ��kernel tainted�� �ľ��档 
*/

