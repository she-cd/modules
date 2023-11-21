#include <linux/init.h>
#include <linux/module.h>
#include <linux/timer.h>


struct timer_list test_timer;

	// ����ʱ�䴦����
static void timer_function(unsigned long data);
	// �ú�ᾲ̬����һ������ timer_name �ں˶�ʱ����
	// ����ʼ���� function, expires, name �� base �ֶΡ�
//DEFINE_TIMER(test_timer, timer_function, 0, 0);

/**
* @description:��ʱ������
* @param {*}
* @return {*}
*/
static void timer_function(unsigned long data)
{
	printk(" This is timer_function \n");
	/**
	* @description: �޸Ķ�ʱֵ�������ʱ����û�м���Ļ���mod_timer �����ἤ�ʱ��
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
	
	
	//��ʼ�� test_timer.expires ��Ϊ��ʱʱ��
	test_timer .expires = jiffies + 1 * HZ;
	//��ʱ��ע�ᵽ�ں����棬������ʱ��
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

