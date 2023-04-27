/* Logical Device to imitate /dev/null */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/types.h>

ssize_t my_read(struct file *fd, char __user *buf, size_t len, loff_t *off);
ssize_t my_write(struct file *fd, const char __user *buf, size_t len, loff_t *off);

static struct file_operations fops;

static int dno = 0;

static int init_test(void)
{
	dno = register_chrdev(dno, "Anantvijay", &fops);
	printk(KERN_ALERT "Hello, Its working\n");
	fops.read = my_read;
	fops.write = my_write;
	return 0;
}


static void  exit_test(void)
{
	unregister_chrdev(dno, "Anantvijay");
	printk(KERN_ALERT "Byeeeeeeeeee\n");
}

module_init(init_test);
module_exit(exit_test);


ssize_t my_read(struct file *fd, char __user *buf, size_t len, loff_t *off)
{
	return 0;	//tell the application that everything is read
}


ssize_t my_write(struct file *fd, const char __user *buf, size_t len, loff_t *off)
{
	return len;	//tell the application that everything is written
}
