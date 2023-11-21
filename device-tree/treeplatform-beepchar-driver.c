/*
 * @Author: topeet
 * @Description: 基于平台设备模型的driver.c,在probe函数中获取硬件资源后，注册一个杂项设备
 */
//初始化头文件
#include <linux/init.h>	
//最基本的文件，支持动态添加和卸载模块。		   
#include <linux/module.h>
//平台设备所需要的头文件?		  
#include <linux/platform_device.h> 
#include <linux/ioport.h>

//文件系统头文件，定义文件表结构（file,buffer_head,m_inode等）
#include <linux/fs.h>	
//包含了copy_to_user、copy_from_user等内核访问用户进程内存地址的函数定义。	 
#include <linux/uaccess.h>	 
//包含了ioremap、iowrite等内核访问IO内存等函数的定义。
#include <linux/io.h>		
#include <linux/kernel.h>
#include <linux/delay.h>
#include <asm/irq.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include<linux/of.h>
#include<linux/of_address.h>



//寄存器地址定义
#define GPIO5_DR 0x020AC000
//存放映射完的虚拟地址的首地址
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
   // copy_from_user(&val, userbuf, bytes);					//拷贝用户空间数据到内核空间

	 char kbuf[64] = {0};
    // copy_from_user 从应用层传递数据给内核层
	if(copy_from_user(&kbuf,userbuf,bytes)!= 0) 
	{
        // copy_from_user 传递失败打印
		printk("copy_from_user error \n ");
		return -1;
	}
    //打印传递进内核的数据
    printk("kbuf is %d\n ",kbuf[0]); 
	if(kbuf[0]==1) //传入数据为1 ，蜂鸣器响
	{
		*vir_gpio5_dr |= (1<<1);
	}
	else if(kbuf[0]==0) //传入数据为0，蜂鸣器关闭
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
 * @brief beep_probe : 与设备信息层（device.c）匹配成功后自动执行此函数，
 * @param inode : 文件索引
 * @param file  : 文件
 * @return 成功返回 0 	    	
 ****************************************************************************************/
int beep_probe(struct platform_device *pdev)
{
	printk("beep_probe\n");
	/*获取硬件资源方法一： 不推荐*/
	//printk("beep_res is %s\n",pdev->resource[0].name);
	//return 0;
	/*获取硬件资源方法二： 推荐*/
    
	//beep_mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);	  /* 获取platform_device的resource资源 */
/*
	if (beep_mem == NULL)
	{
		printk("platform_get_resource is error\n");
		return -EBUSY;
	}
	printk("beep_res start is 0x%x \n", beep_mem->start);
	printk("beep_res end is 0x%x \n", beep_mem->end);
   */
	//映射GPIO资源
	/*vir_gpio5_dr = ioremap(beep_mem->start, 4);
	if (vir_gpio5_dr == NULL)
	{
		printk("GPIO5_DR ioremap is error \n");
		return EBUSY;
	}
	printk("GPIO5_DR ioremap is ok \n");
   */
   
	/*********************通过函数获取硬件资源**************************/
	test_device_node = of_find_node_by_path("/test-tree-platform");//获得设备节点
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
	/*********************映射物理地址*************************/
	vir_gpio5_dr=of_iomap(pdev->dev.of_node,0);
     if(vir_gpio5_dr==NULL){
		 printk("of_iomap is error\n");
		 return-1;
	 }



	//注册char设备

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




//?platform?驱动结构体
struct platform_driver beep_driver = {
	.probe = beep_probe,		/* 探测函数 */
	.remove = beep_remove,
	.driver = {
		.owner = THIS_MODULE,
		.name = "test",
		.of_match_table=of_match_table_test
	},
	.id_table = &beep_idtable,	/* 匹配这里，如果匹配成功，则不会再比较driver.name成员 */

};


static int beep_driver_init(void)
{
	int ret = 0;
	//?platform驱动注册到?Linux?内核
	ret = platform_driver_register(&beep_driver);
	if (ret < 0)
	{
		printk("platform_driver_register error \n");
	}
	printk("platform_driver_register ok \n");

	return 0;
}
/**
  设备模块注销
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

module_init(beep_driver_init);	  /* 驱动入口函数 */
module_exit(beep_driver_exit);	  /* 驱动出口函数 */
MODULE_LICENSE("GPL");	  

/* 
  从2.4.10版本内核开始，模块必须通过MODULE_LICENSE宏声明
  此模块的许可证，否则在加载此模块时，会收到内核被污染
 “kernel tainted” 的警告。 
*/

