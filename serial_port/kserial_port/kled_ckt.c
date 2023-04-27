/* Driver for the LED circuit */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/serial_reg.h>
#include <linux/kdev_t.h>

#define DYNAMIC_ALLOC
#define SERIAL_PORT 0x03F8

int my_open(struct inode *inodp, struct file *filp);
ssize_t my_write(struct file *fd, const char __user *buf, size_t len, loff_t *off);

static struct file_operations fops = {
	.open = my_open,
	.write = my_write,
};
static unsigned int dev_no = 0;
//static unsigned int minor;

int my_open(struct inode *inodp, struct file *filp)
{
	filp->private_data = (void *)iminor(inodp);	
	return 0;
}
	

ssize_t my_write(struct file *fd, const char __user *buf, size_t len, loff_t *off)
{
	unsigned char port_value, data, mask = 0x01;
	unsigned int i;

	switch((unsigned int)fd->private_data)
	{
		case 0: data = 0x88;
				break;

		case 1: data = 0x11;
				break;

		default: data = 0xAA;
	}
	
	port_value = inb(SERIAL_PORT + UART_MCR);	//get current data on serial port

	for(i = 0; i < 8; i++)
	{
		if((data >> i) & mask)
			port_value |= UART_MCR_RTS;		//Glow the LED
		else
			port_value &= (~UART_MCR_RTS);	//reset the LED
		
		//Clock
		port_value |= UART_MCR_DTR;
		outb(port_value, SERIAL_PORT + UART_MCR);
		port_value &= (~UART_MCR_DTR);
		outb(port_value, SERIAL_PORT + UART_MCR);
	}

	return len;
}


static int init_led_ckt(void)
{
	dev_no = register_chrdev(dev_no, "LED Circuit", &fops);
	printk(KERN_ALERT "Hello, Its working\n");
	return 0;
}


static void  exit_led_ckt(void)
{
	unregister_chrdev(dev_no, "test_v2");
	printk(KERN_ALERT "Byeeeeeeeeee\n");
}


module_init(init_led_ckt);
module_exit(exit_led_ckt);
