/* Module to be loaded into the kernal */

#include <linux/init.h>
#include <linux/module.h>

static int khello_init(void)
{
	printk(KERN_ALERT "Hello, Its working\n");
	return 0;
}

static void khello_exit(void)
{
	printk(KERN_ALERT "Byeeeeeeeeee\n");
}

module_init(khello_init);
module_exit(khello_exit);
