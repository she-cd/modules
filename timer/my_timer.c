#include <linux/init.h>
#include <linux/module.h>
#include <linux/timer.h>


struct timer_list test_timer;

	// 声明时间处理函数
static void timer_function(unsigned long data);
	// 该宏会静态创建一个名叫 timer_name 内核定时器，
	// 并初始化其 function, expires, name 和 base 字段。
//DEFINE_TIMER(test_timer, timer_function, 0, 0);

/**
* @description:超时处理函数
* @param {*}
* @return {*}
*/
static void timer_function(unsigned long data)
{
	printk(" This is timer_function \n");
	/**
	* @description: 修改定时值，如果定时器还没有激活的话，mod_timer 函数会激活定时器
	* @param {1} *
	* @return {*}
	*/
	mod_timer(&test_timer, jiffies + 1 * HZ);
}

static int hello_init(void)
{
	printk("hello world ! \n");
	init_timer(&test_timer);
	test_timer.function=timer_function;
	
	
	//初始化 test_timer.expires 意为超时时间
	test_timer .expires = jiffies + 1 * HZ;
	//定时器注册到内核里面，启动定时器
	add_timer(&test_timer);
	
	return 0;
}



static void hello_exit(void){
	del_timer_sync(&test_timer);
	printk("timer_exit is ok\n");
}

module_init(hello_init);

module_exit(hello_exit);

MODULE_LICENSE("GPL");

