/*
 * Char driver for 2.6
 */
#include <linux/kernel.h>			/* printk */
#include <linux/init.h>				/* module_init, module_exit */
#include <linux/module.h>
#include <linux/types.h>			/* dev_t */
#include <linux/fs.h>				/* register_chr... */
#include <linux/cdev.h>

#include <linux/slab.h>				/* kmalloc */

static int __init char_driver_init (void)
{
	void *ptr;
	ptr = __get_free_pages(GFP_KERNEL, 4);  /* 2^4 = 16 pages */
	if( ptr == NULL ){
		printk(KERN_ALERT "Failed to get memory\n");
		return -1;
	}
	printk(KERN_ALERT "Physical address is %u\n", virt_to_phys(ptr));
	return 0;
}

void __exit char_driver_cleanup (void)
{
	return;
}


module_init(char_driver_init);
module_exit(char_driver_cleanup);

MODULE_AUTHOR("Padmanabha S");
MODULE_DESCRIPTION("SIMPLE CHAR DRIVER IMPLEMENTATION");
MODULE_LICENSE("GPL");
