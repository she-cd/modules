#include <linux/init.h>
#include <linux/module.h>
static int add;

module_param(add, int, S_IRUGO);




static int param_init(void){
//KERN_EMERG为打印级别 
	printk(KERN_EMERG "THIS is MODULE_INIT\n");
	pr_warn("THIS is pr_warn!\n");


	printk("add value is %d\n",add);
	return 0;

}

static void param_exit(void){

	printk("THIS is MODULE_EXIT\n");

}
module_init(param_init);
module_exit(param_exit);
MODULE_LICENSE("GPL");

