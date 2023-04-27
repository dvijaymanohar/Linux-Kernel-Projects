#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/serial_reg.h>
MODULE_LICENSE("Dual BSD/GPL");

#define BASE_ADDR 0x3F8
static char step_char;
static int major;
static struct file_operations fops;
ssize_t my_write(struct file *, const char __user *, size_t, loff_t *);


static int stepper_module_init(void)
{

	fops.write = my_write;
	if((major = register_chrdev(major, "stepper", &fops)) == -1)
		return -1;
	printk(KERN_ALERT "ANNOUNCEMENT: Stepper motor module register with major no. %x.\n", major);
	return 0;
}

static void stepper_module_exit(void)
{
	printk(KERN_ALERT "ANNOUNCEMENT:Stepper motor module unregistered.\n");
	unregister_chrdev(major, "stepper");
}

module_init(stepper_module_init);
module_exit(stepper_module_exit);
		
void resetbit(unsigned char bit, unsigned port)
{
	unsigned char receive = 0;
	
	receive = inb(port);
	receive = receive | bit;
	outb(receive, port);
}

void setbit(unsigned char mask, unsigned port)
{
	unsigned char receive = 0;
	
	receive = inb(port);
	receive = (receive & mask);
	outb(receive, port);
}


void outport(unsigned char out_pattern, size_t length)
{
	unsigned int bit, mask = 0x80, i;
	
	for(i = 0; i < length && mask != 0; i++)
	{
		bit = out_pattern & mask ? 1 : 0;
		if(bit == 1)
		{
			resetbit(UART_MCR_RTS, BASE_ADDR + UART_MCR);
			resetbit(UART_MCR_DTR, BASE_ADDR + UART_MCR);
			setbit(~UART_MCR_DTR, BASE_ADDR + UART_MCR);
		}
		else if(bit == 0)
		{
			setbit(~UART_MCR_RTS, BASE_ADDR + UART_MCR);
			resetbit(UART_MCR_DTR, BASE_ADDR + UART_MCR);
			setbit(~UART_MCR_DTR, BASE_ADDR + UART_MCR);
		}
		mask >>= 1;
	}
	return;
}

void m_delay(void)
{
	msleep(200);
	return;
}

ssize_t my_write(struct file *fd, const char __user * buf, size_t len, loff_t *off)
{

	unsigned long ret = 0;
	unsigned char length = 8;
	
	ret = copy_from_user(&step_char, buf, 1);
	printk(KERN_DEBUG "The value of step_char is %x \n", step_char);
	outport(step_char, length);
//	m_delay();
	return len;
}

