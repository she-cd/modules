#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/ioport.h>
#include <linux/device.h>

struct resource test_resource[]={
[0]={	.start=0x200AC000,
	.end=0x200AC003,
	.name="GPIO5_DR",
	.flags=IORESOURCE_MEM,
}
};
void platform_release(struct device *dev){

	printk("platform_release is ok\n");

}


struct platform_device platform_device_test={
	.name="scd_platform",
	.id=-1,
	.dev={
		.release=platform_release
	},

	.num_resources=ARRAY_SIZE(test_resource),
	.resource=test_resource

};



static int device_init(void){
	platform_device_register(&platform_device_test);
	
	printk("device_init is ok\n");
	return 0;
}

static void device_exit(void){

	platform_device_unregister(&platform_device_test);
		
	printk("device_exit is ok\n");

}

module_init(device_init);
module_exit(device_exit);
MODULE_LICENSE("GPL");

