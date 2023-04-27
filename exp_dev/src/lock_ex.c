/*
 * Char driver for 2.6
 */
#include <linux/kernel.h>			/* printk */
#include <linux/init.h>				/* module_init, module_exit */
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/types.h>			/* dev_t */
#include <linux/fs.h>				/* register_chr... */
#include <linux/cdev.h>

#include <linux/slab.h>				/* kmalloc */
#include <linux/semaphore.h>
#include <asm/atomic.h>
//#include <asm/semaphore.h>
#include <asm/uaccess.h>			/* copy_to_user.... */

#define MAX_SIZE		(128 * 1024)

//#define TEST

#ifdef TEST
#include <linux/delay.h>
#endif

/*
 * MY Device Structure
 */

struct exp_dev {
	void * prv_ptr;
	int size;
	dev_t dev_no;

	/*
         *Embed cdev in device structure, so that one can
	 *get back pointer to device structure using container_of macro
	 */ 
	struct cdev cdev;

	int devices;
	atomic_t ref_cnt;

	struct semaphore sem;	/* Mutual Exclusive operation */
};

struct exp_dev	exp_dev;
struct exp_dev *dev = &exp_dev;

/*
 * read
 */
ssize_t exp_read( struct file *file_ptr, char __user * user_buf,
		   size_t count, loff_t *f_ops)
{
	struct exp_dev *dev = file_ptr->private_data; /* refer open */
	ssize_t retval = 0;
	void *cur_pos;

	if(down_interruptible(&dev->sem))
		return -ERESTARTSYS;


	if(*f_ops >= dev->size)
		goto out;
	if(*f_ops + count > dev->size)
		count = dev->size - *f_ops;

	if(dev == NULL || dev->prv_ptr == NULL)
		goto out;
	cur_pos = (void *)((char *)dev->prv_ptr + *f_ops);

	printk(KERN_DEBUG "cur %p, count %d, f_pos %lld\n",\
						cur_pos, count, *f_ops);

	if( copy_to_user(user_buf, cur_pos, count)){
		retval = -EFAULT;
		goto out;
	}

	*f_ops += count;
	retval = count;

#ifdef TEST
	mdelay(1000);
#endif //TEST

out:

	up(&dev->sem);

	return retval;
}

/*
 * write
 */ 
ssize_t exp_write( struct file *file_ptr, const char __user * user_buf,
		   size_t count, loff_t *f_ops)
{
	struct exp_dev *dev = file_ptr->private_data; /* refer open */
	ssize_t retval = 0;
	void *cur_pos;


	if(down_interruptible(&dev->sem))
		return -ERESTARTSYS;

	if(*f_ops >= dev->size)
		goto out;
	if(*f_ops + count > dev->size)
		count = dev->size - *f_ops;

	if(dev == NULL || dev->prv_ptr == NULL)
		goto out;
	cur_pos = (void *)((char *)dev->prv_ptr + *f_ops);

	printk(KERN_DEBUG "cur %p, count %d, f_pos %lld\n",\
						  cur_pos, count, *f_ops);

	if( copy_from_user( cur_pos, user_buf, count)){
		return -EFAULT;
		goto out;
	}
	printk(KERN_ALERT " string at beg is %s \n", (char *)dev->prv_ptr);

	*f_ops += count;
	retval = count;

#ifdef TEST
	mdelay(2000);
#endif

out:

	up(&dev->sem);

	return retval;
}

/*
 * open
 */ 
int exp_open(struct inode *inode, struct file *file_ptr)
{
	struct exp_dev *dev;
	int cnt;
	dev = container_of(inode->i_cdev, struct exp_dev, cdev);
	file_ptr->private_data = dev;	/* for other methods */

	atomic_inc(&dev->ref_cnt);
	cnt = atomic_read(&dev->ref_cnt);
	printk(KERN_INFO "reference count reads %d\n", cnt);
	return 0;
}

/*
 * release
 */ 
int exp_release(struct inode *inode, struct file *file_ptr)
{
	
	atomic_dec(&dev->ref_cnt);
	return 0;
}

/*
 * llseek
 */ 
loff_t exp_llseek(struct file *file_ptr, loff_t off, int whence)
{
	struct exp_dev *dev = file_ptr->private_data;
	loff_t newpos;

	printk(KERN_DEBUG "OLD f_pos %lld\n", file_ptr->f_pos);
	switch(whence) {
		case 0:	/* SEEK_SET */
		  newpos = off;
		  break;

		case 1:	/* SEEK_CUR */
		  newpos = file_ptr->f_pos + off;
		  break;

		case 2:	/* SEEK_END */
		  newpos = dev->size + off;
		  break;

		default:
		  return -EINVAL;
	}
	if(newpos < 0)
		return -EINVAL;
	file_ptr->f_pos = newpos;

	printk(KERN_DEBUG "NEW f_pos %lld\n", file_ptr->f_pos);
	return newpos;
}

/*
 * ioctl
 */ 
int exp_ioctl (struct inode *inode, struct file *file_ptr, unsigned int cmd,
		unsigned long arg)
{
	return 0;
}

/*
 * Char Interface for Device
 */

struct file_operations exp_fops = {
	.owner   = THIS_MODULE,
	.read    = exp_read,
	.write   = exp_write,
	.llseek  = exp_llseek,
	.ioctl   = exp_ioctl,
	.open    = exp_open,
	.release = exp_release,
};

static int __init char_driver_init (void)
{
	int major = 0,minor = 1;
	int err = 0;
	void *prv_ptr;
	dev_t dev_no;

	dev->devices = 1;
	atomic_set(&dev->ref_cnt, 0);

	/*
 	 * Upper limit for kmalloc is 128 KB
 	 * if your code need to be complete portable
 	 */
	dev->size = MAX_SIZE;
	prv_ptr = kmalloc( MAX_SIZE, GFP_KERNEL );
	if( prv_ptr == NULL ){
		printk(KERN_ERR "Unable to allocate memory\n");
		return -1;
	}
	dev->prv_ptr = prv_ptr;
 
	if(major){
		dev_no = MKDEV(major, minor);
		if( register_chrdev_region(dev_no,
					   dev->devices, "MY_CHAR_DEV" ) < 0 )
			return -1;
	}
	else
	{
		if( alloc_chrdev_region(&dev_no, minor, 
					dev->devices, "MY_CHAR_DEV") < 0)
			return -1;
	}
	dev->dev_no = dev_no;

	cdev_init(&dev->cdev, &exp_fops);
	dev->cdev.owner = THIS_MODULE;
	dev->cdev.ops = &exp_fops;


	init_MUTEX(&dev->sem);

	err = cdev_add(&dev->cdev, dev_no, dev->devices);
	if( err ){
		printk(KERN_NOTICE "Error Adding the char driver\n");
		return -1;
	}
	printk(KERN_INFO "Char driver, major no %d, minor no %d registered\n",\
		MAJOR(dev_no), MINOR(dev_no));
	return 0;
}

void __exit char_driver_cleanup (void)
{
	kfree(dev->prv_ptr);
	cdev_del(&dev->cdev);
	unregister_chrdev_region(dev->dev_no, dev->devices);
}

module_init(char_driver_init);
module_exit(char_driver_cleanup);

MODULE_AUTHOR("Padmanabha S");
MODULE_DESCRIPTION("SIMPLE CHAR DRIVER IMPLEMENTATION");
MODULE_LICENSE("GPL");

