#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/ioport.h>


struct resource scd_resource[]={
[0]={.start=0x20AC000,
	.end=0x20AC003,
	.name="GPIO5_DR",
	.flags=IORESOURCE_MEM,
}


};



void	platform_device_release(struct device *dev){
	printk("platform_device_release is ok!!!\n");
	
}


struct platform_device scd_platform={
	.name="scd_platform",
	.id=-1,
	.dev={
	.release=platform_device_release},
	.num_resources=ARRAY_SIZE(scd_resource),
	.resource=scd_resource,

};



static int device_init(void){
	int ret;
	
	ret=platform_device_register(&scd_platform);
	if(ret<0){
		printk("platform_device_register is error!!!\n");
		return 0;
	}


	printk("device_init is ok\n");
	return 0;
}

static void device_exit(void){

	platform_device_unregister(&scd_platform);
		
	printk("device_exit is ok\n");

}

module_init(device_init);
module_exit(device_exit);
MODULE_LICENSE("GPL");

