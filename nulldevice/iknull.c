#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/types.h>
//MODULE_LICENCE("Dual BSD/GPL");

static struct file_operations fops;
ssize_t my_read(struct file_operations *, char __user *, size_t, loff_t *);
ssize_t my_write(struct file_operations *, const char __user *, size_t, loff_t *);

#define major 0

static int simple_hello_init(void)
{
	int reg;

	fops.read = my_read;
	fops.write = my_write;
	printk(KERN_ALERT "Yay! I'm Loaded\n");
	if((reg = register_chrdev(major, "aknull", &fops)) == -1)
		return -1;
	return 0;
}

static void simple_hello_exit(void)
{
	printk(KERN_ALERT "Booh!! You Suck Smart Alec!\n");
	unregister_chrdev(major, "aknull");
}

module_init(simple_hello_init);
module_exit(simple_hello_exit);

ssize_t my_read(struct file_operations *fd, char __user * buf, size_t len, loff_t *off)
{
	return 0;
}

ssize_t my_write(struct file_operations *fd, const char __user * buf, size_t len, loff_t *off)
{
	return len;
}
