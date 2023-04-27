#include <linux/types.h>
#include <linux/fs.h>


MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("SCULL : Simple character utility for loading localities");
MODULE_AUTHOR("LDD");

//if scull_major = 0, then go for dynamic allocation

if (scull_major) {
	dev = MKDEV(scull_major, scull_minor);
	result = register_chrdev_region(dev, scull_nr_devs, "scull");
} 
else {
	result = alloc_chrdev_region(&dev, scull_minor, scull_nr_devs, "scull");
	scull_major = MAJOR(dev);
}

if (result < 0) {
	printk(KERN_WARNING "scull: can't get a major number for ur device %d\n", scull_major);
	return result;
}

struct file_operations scull_fops = {

    .owner		=	THIS_MODULE,
    .llseek		=	scull_llseek,
	.read		=	scull_read,
	.write		=	scull_write,
	.ioctl		=	scull_ioctl,
	.open		=	scull_open,
	.release	=	scull_release,
	
};


							





//Module Cleanup function
//	unregister the device numbers
		void unregister_chrdev_region(dev_t first, unsigned int count);

