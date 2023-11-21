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
#include<linux/of_gpio.h>
#include<linux/gpio.h>


int ret;
int beep_gpio;

#define FIRST_DEV MKDEV(170, 5)
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
   int n=0;
	 char kbuf[64] = {0};
	if(copy_from_user(&kbuf,userbuf,bytes)!= 0) 
	{
		printk("copy_from_user error \n ");
		return -1;
	}
    printk("kbuf is %d\n ",kbuf[0]); 
	if(kbuf[0]==1)
	{
		gpio_set_value(beep_gpio, 1);
			n=gpio_get_value(beep_gpio);
   			printk("n1 value is %d",n);
	}
	else if(kbuf[0]==0){
		gpio_set_value(beep_gpio, 0);
		n=gpio_get_value(beep_gpio);
  		 printk("n0 value is %d",n);
		}
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


int beep_probe(struct platform_device *pdev)
{
	printk("beep_probe\n");
	int num;
	test_device_node = of_find_node_by_path("/led-test");
	if( test_device_node == NULL){
		printk("of_find_node_by_path is error\n");
		return-1;
	}
    printk("of_find_node_by_path is ok\n");
/**************right_DATA00_gpios***************/
	beep_gpio=of_get_named_gpio(test_device_node,"right_DATA00_gpios",0);
	if(beep_gpio<0){
		printk("beep_gpio is error\n");
		return -1;        
	  }
	printk("right_DATA00_gpios is %d\n",beep_gpio);

	ret=gpio_request(beep_gpio,"beep");  
	if(ret<0){
		printk(" gpio_request is error \n");
		return -1;        
	  }
	gpio_direction_output(beep_gpio,1);

	num=gpio_get_value(beep_gpio);
   printk("num value is %d",num);

	

    int retchar;
    int retcdev;
    retchar = register_chrdev_region(FIRST_DEV, 1, "ledtest");
	 if (retchar) {
        printk(KERN_ERR "Unable to register firstdrv\n");
        goto err_reg;
    }
    cdev_init(&firstdrv_cdev, &firstdrv_ops);

    retcdev = cdev_add(&firstdrv_cdev, FIRST_DEV, 1);
    if (retcdev) {
        printk(KERN_ERR "Unable to add cdev\n");
        goto err_add_cdev;
    }
    class_register(&firstdrv_class);
	device_create(&firstdrv_class, NULL, FIRST_DEV, NULL, "ledtest");
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
	{.compatible="led_test"},
		{},
};

struct platform_driver beep_driver = {
	.probe = beep_probe,		
	.remove = beep_remove,
	.driver = {
		.owner = THIS_MODULE,
		.name = "test",
		.of_match_table=of_match_table_test
	},
	.id_table = &beep_idtable,	

};


static int beep_driver_init(void)
{
	int ret = 0;
	
	ret = platform_driver_register(&beep_driver);
	if (ret < 0)
	{
		printk("platform_driver_register error \n");
	}
	printk("platform_driver_register ok \n");

	return 0;
}

static void beep_driver_exit(void)
{
	platform_driver_unregister(&beep_driver);
	cdev_del(&firstdrv_cdev);
	device_destroy(&firstdrv_class, FIRST_DEV);
	unregister_chrdev_region(FIRST_DEV, 1);
   gpio_free(beep_gpio);
	class_destroy(&firstdrv_class);
	
	printk("gooodbye! \n");
}

module_init(beep_driver_init);	  
module_exit(beep_driver_exit);	  
MODULE_LICENSE("GPL");	  


