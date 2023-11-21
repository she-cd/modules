#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/ioport.h>
struct resource *resource_scd;

int scd_platform_driver_probe(struct platform_device *pdev){
	resource_scd=platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if(resource_scd == NULL){
		printk("platform_get_resource is error\n");
		return 0;

	}
	/***********方法一***********/

	printk("platform_device_resource name is %s\n",pdev->resource[0].name);

	/************方法二*************/
	printk("platform_get_resource is ok\n");
	printk("platform_get_resource name is %s\n",resource_scd->name);
	printk("platform_get_resource start is 0x%x\n",resource_scd->start);
	printk("platform_get_resource end is 0x%x\n",resource_scd->end);
	printk("platform_driver_probe is ok\n");
	return 0;
}
int scd_platform_driver_remove(struct platform_device *pdev){
	printk("platform_driver_remove is ok\n");
	return 0;
}
const struct platform_device_id scd_id_table={
	.name="scd_platform"		

};


struct platform_driver scd_platform_driver={
	.probe=scd_platform_driver_probe,
	.remove=scd_platform_driver_remove,
	.driver={
		.name="scd_platform",
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

