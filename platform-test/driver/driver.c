#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/ioport.h>

struct resource *resource_test;


int platform_probe(struct platform_device *pdev){


	printk("platform_device name is %s\n",pdev->name);

	resource_test=platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if(resource_test == NULL){

	printk("error\n");
	return 0;
	}
	printk("platform_device_redource[0]_name is %s\n",resource_test->name);
	printk("platform_device_redource[0]_start is ox%x\n",resource_test->start);
	printk("platform_device_redource[0]_end is ox%x\n",resource_test->end);
	printk("platform_probe is ok\n");
	return 0;
}
int platform_remove(struct platform_device *pdev){
	printk("platform_remove is ok\n");
	return 0;

}


const struct platform_device_id scd_id_table={
	.name="scd_platform",

};



struct platform_driver scd_platform_driver={
	.probe=platform_probe,
	.remove=platform_remove,
	.driver={
		.name="scd_123",
		.owner=THIS_MODULE,
	},
	.id_table=&scd_id_table,
};




static int drive_testr_init(void){
	platform_driver_register(&scd_platform_driver);
	printk("driver_init is ok\n");
	return 0;

}

static void driver_test_exit(void){
	platform_driver_unregister(&scd_platform_driver);

	printk("driver_exit is ok\n");

}
module_init(drive_testr_init);
module_exit(driver_test_exit);
MODULE_LICENSE("GPL");

