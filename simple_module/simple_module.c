#include <linux/init.h>
#include <linux/module.h>

static int simple_hello_init(void)
{
	printk(KERN_ALERT "Yay! I'm Loaded\n");
	return 0;
}

static void simple_hello_exit(void)
{
	printk(KERN_ALERT "Booh!! I'm Unloaded\n");
}

module_init(simple_hello_init);
module_exit(simple_hello_exit);
