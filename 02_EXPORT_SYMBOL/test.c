#include <linux/init.h>
#include <linux/module.h>


extern int add(int a,int b);



static int test_init(void){
//KERN_EMERG为打印级别 
	printk(KERN_EMERG "THIS is test_INIT\n");
//此为printk打印级别的封装函数，作用相同

	pr_emerg("THIS is pr_emerg!\n");


	printk("add value is %d\n",add(1,3));
	return 0;

}

static void test_exit(void){

	printk("THIS is test_EXIT\n");

}
module_init(test_init);
module_exit(test_exit);
MODULE_LICENSE("GPL");

