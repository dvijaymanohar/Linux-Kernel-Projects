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
#define LED_BITS 8
static char led_char, pattern;
static int major;
static struct file_operations fops;
ssize_t my_write(struct file *, const char __user *, size_t, loff_t *);


static int led_module_init(void)
{

	fops.write = my_write;
	printk(KERN_ALERT "ANNOUNCEMENT: LED MODULE LOADED.\n");
	if((major = register_chrdev(major, "ledev", &fops)) == -1)
		return -1;
	return 0;
}

static void led_module_exit(void)
{
	printk(KERN_ALERT "ANNOUNCEMENT:LED MODULE UNLOADED.\n");
	unregister_chrdev(major, "ledev");
}

module_init(led_module_init);
module_exit(led_module_exit);
		
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

unsigned char select_pattern(unsigned char option)
{
	unsigned char s_pattern;

	s_pattern = (option & 0xE0);
	s_pattern >>= 5;
	switch(s_pattern)
	{
		case 0x00:
				s_pattern = 0x80;
				break;
		case 0x01:
				s_pattern = 0xAA;
				break;
		case 0x02:
				s_pattern = 0x33;
				break;
		case 0x03:
				s_pattern = 0x0F;
				break;
		case 0x04:
				s_pattern = 0xE7;
				break;
		case 0x05:
				s_pattern = 0x99;
				break;
		case 0x06:
				s_pattern = 0xFF;
				break;
		case 0x07:
				s_pattern = 0x7E;
				break;
	}
	return s_pattern;
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
	msleep(600);
	return;
}

ssize_t my_write(struct file *fd, const char __user * buf, size_t len, loff_t *off)
{

	unsigned long ret = 0;
	unsigned char val = 0x00, clone_pattern,  repeat_count, counter, move;
	size_t length = 8;
	
	ret = copy_from_user(&led_char, buf, 1);
	printk(KERN_ALERT "The value of led_char is %x \n", led_char);
	pattern = select_pattern(led_char);
	repeat_count = led_char & 0x0F;
	clone_pattern = pattern;
	printk(KERN_ALERT "The selected pattern is %x \n", pattern);
	for(counter = 0; counter <= repeat_count; counter++)
	{
/*		for(move = 1; move <= LED_BITS; move++)
		{*/
			outport(clone_pattern, length);
/*			if((led_char & 0x10) == 0x10)
			{
				clone_pattern = (clone_pattern >> 1) | (mask << (8 - move));
			}
			else if((led_char & 0x10) == 0x00)
			{
				clone_pattern = (clone_pattern << 1) | (mask >> (8 - move));
			}*/
			m_delay();
			outport(val, length);
			m_delay();
	//	}
/*		m_delay();
		clone_pattern >>= 1;
		outport(clone_pattern, length);
		if((counter % 2) != 0)
		{
			mask = pattern;
			clone_pattern = 0x00;
		}
		else if((counter % 2) == 0)
		{
			clone_pattern = pattern;
			mask = 0x00;
		}*/
	}
	return len;
}

