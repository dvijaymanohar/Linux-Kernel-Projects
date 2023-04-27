#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <asm/uaccess.h>
MODULE_LICENCE("Dual BSD/GPL");

static char my_char = 'A';
static int major;
static struct file_operations fops;
ssize_t my_read(struct file *, char __user *, size_t, loff_t *);
ssize_t my_write(struct file *, const char __user *, size_t, loff_t *);


static int chardev_init(void)
{

	fops.read = my_read;
	fops.write = my_write;
	printk(KERN_ALERT "Yay! I'm Loaded\n");
	if((major = register_chrdev(major, "unichrdev", &fops)) == -1)
		return -1;
	return 0;
}

static void chardev_exit(void)
{
	printk(KERN_ALERT "Booh!! You Suck Smart Alec!\n");
	unregister_chrdev(major, "unichrdev");
}

module_init(chardev_init);
module_exit(chardev_exit);

ssize_t my_read(struct file *fd, char __user * buf, size_t len, loff_t *off)
{
	int i;
	unsigned long val = 0;
	
	for(i = 0; i < len; i++)
		val = copy_to_user((buf + i), &my_char, 1);
		
	return len;
}

ssize_t my_write(struct file *fd, const char __user * buf, size_t len, loff_t *off)
{

	unsigned long ret = 0;
	ret = copy_from_user(&my_char, &buf[len - 1], 1);
	return len;
}
