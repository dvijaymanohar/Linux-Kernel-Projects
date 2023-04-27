/* Logical device that is modification of null driver */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

ssize_t my_read(struct file *fd, char __user *buf, size_t len, loff_t *off);
ssize_t my_write(struct file *fd, const char __user *buf, size_t len, loff_t *off);

static struct file_operations fops;
static int dev_no = 0;
static char my_char = 'a';

static int init_test(void)
{
	dev_no = register_chrdev(dev_no, "test_v2", &fops);
	printk(KERN_ALERT "Hello, Its working\n");
	fops.read = my_read;
	fops.write = my_write;
	return 0;
}


static void  exit_test(void)
{
	unregister_chrdev(dev_no, "test_v2");
	printk(KERN_ALERT "Byeeeeeeeeee\n");
}

module_init(init_test);
module_exit(exit_test);


ssize_t my_read(struct file *fd, char __user *buf, size_t len, loff_t *off)
{
	unsigned long bytes = 0;
	size_t i;
	
	for(i = 0; i < len; i++)
		bytes = copy_to_user(buf + i, &my_char, 1);
	
	return len;	
}


ssize_t my_write(struct file *fd, const char __user *buf, size_t len, loff_t *off)
{
	unsigned long bytes = 0;
	
	bytes = copy_from_user(&my_char, buf + len - 1, 1);
			
	return len;	//tell the application that everything is written
}
