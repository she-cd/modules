#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/ioport.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_address.h>
int size;
u32 out_values[2]={0};
unsigned int *numscd;
struct device_node*scd_device_node;

struct property * scd_property;



int beep_tree_probe(struct platform_device *pdev){
	printk("beep_tree_probe is ok\n");
/****************************直接获取节点********************************/
	printk("pdev_node_name is %s\n",pdev->dev.of_node->name);
	
/*********************查询节点的of函数**************************/
	scd_device_node=of_find_node_by_path("/test");
	if(scd_device_node ==NULL){

	printk("of_find_node_by_path is error\n");
	return 0;

	}
	printk("of_find_node_by_path name is %s\n",scd_device_node->name);


/************************查询属性的of函数*************************************/
	/*****************查找查找指定的属性************************/
	scd_property=of_find_property(scd_device_node, "compatible", &size);
	if(scd_property ==NULL){
	
		printk("of_find_property is error\n");
		return 0;
	
	}
	printk("of_find_property name is %s\n",scd_property->name);
	printk("of_find_property name is %s\n",scd_property->value);

	

/************************读取u32类型的reg值******************************/
	of_property_read_u32_array(scd_device_node,"reg",out_values, 2);
	
	printk("of_property_read_u32_array value is 0x%x\n",out_values[0]);
/******************************内存映射**************************************/
 	numscd=of_iomap(scd_device_node,0);
	if(numscd ==NULL){
	
		printk("of_iomap is error\n");
		return 0;
	
	}
	printk("of_iomap is ok\n");




return 0;

}


int beep_tree_remove(struct platform_device *pdev){
	printk("beep_tree_remove is ok\n");
	return 0;
}

const struct of_device_id driver_of_match_table[]={
	{.compatible="tree_test"},
	{}

};

const struct platform_device_id test_id_table={
	.name="awdawda"		
};

struct platform_driver beep_tree_driver={
	.probe=beep_tree_probe,
	.remove=beep_tree_remove,
	.driver={
		.name="scd_platform",
		.owner=THIS_MODULE,
		.of_match_table=driver_of_match_table,

	},
	.id_table=&test_id_table,
};
	
	
	
	
static int beep_tree_init(void){
	
	platform_driver_register(&beep_tree_driver);
	printk("driver_init is ok\n");
	return 0;
	
}
	
static void beep_tree_exit(void){
	platform_driver_unregister(&beep_tree_driver);
	printk("driver_exit is ok\n");
	
}
module_init(beep_tree_init);
module_exit(beep_tree_exit);
MODULE_LICENSE("GPL");
	

