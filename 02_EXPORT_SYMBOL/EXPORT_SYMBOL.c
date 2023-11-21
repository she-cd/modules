#include <linux/init.h>
#include <linux/module.h>


static int add(int a,int b){

	return a+b;

}
EXPORT_SYMBOL(add);



static int symbol_test_init(void){
//KERN_EMERG为打印级别 
	printk(KERN_EMERG "THIS is symbol_test_INIT\n");
//此为printk打印级别的封装函数，作用相同

	pr_emerg("THIS is pr_emerg!\n");


	//printk("add value is %d\n",add);
	return 0;

}

static void symbol_test_exit(void){

	printk("THIS is symbol_test_EXIT\n");

}
module_init(symbol_test_init);
module_exit(symbol_test_exit);
MODULE_LICENSE("GPL");

