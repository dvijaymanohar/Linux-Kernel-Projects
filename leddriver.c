#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/serial_reg.h>
#include <asm/io.h>
#include <asm/uaccess.h>

#define BASE_SERIAL_PORT 0x3f8

struct file_operations fops;
static int major = 0;

void set_value_UART_MCR_DTR(void)
{
        unsigned char value_mcr;
        value_mcr = inb(BASE_SERIAL_PORT + UART_MCR);
        value_mcr = value_mcr & ~ UART_MCR_LOOP;
        value_mcr = value_mcr | UART_MCR_DTR;
        outb(value_mcr, BASE_SERIAL_PORT + UART_MCR);
}

void reset_value_UART_MCR_DTR(void)
{
        unsigned char value_mcr;
        value_mcr = inb(BASE_SERIAL_PORT + UART_MCR);
        value_mcr = value_mcr & ~ UART_MCR_LOOP;
        value_mcr = value_mcr & ~UART_MCR_DTR;
        outb(value_mcr, BASE_SERIAL_PORT + UART_MCR);
}
void set_value_UART_MCR_RTS(void)
{
        unsigned char value_mcr;
        value_mcr = inb(BASE_SERIAL_PORT + UART_MCR);
        value_mcr = value_mcr & ~ UART_MCR_LOOP;
        value_mcr = value_mcr | UART_MCR_RTS;
        outb(value_mcr, BASE_SERIAL_PORT + UART_MCR);
}
void reset_value_UART_MCR_RTS(void)
{
        unsigned char value_mcr;
        value_mcr = inb(BASE_SERIAL_PORT + UART_MCR);
        value_mcr = value_mcr & ~ UART_MCR_LOOP;
        value_mcr = value_mcr & ~UART_MCR_RTS;
        outb(value_mcr, BASE_SERIAL_PORT + UART_MCR);
}

void clock_drive(void)
{
       set_value_UART_MCR_DTR();
       reset_value_UART_MCR_DTR();
}

void reset_all(void)
{
	int i;
	for(i = 0; i < 8; i++)
	{
        reset_value_UART_MCR_RTS();
	   	clock_drive(); 
	}
}

void delay(void)
{
        unsigned int i;
        for(i = 0; i < 536870912; i++);
}

void sequence_one(void)
{
        int i;
        reset_all();
        delay();	
        for(i = 0; i < 4; i++)
        {
        	  set_value_UART_MCR_RTS();
		      clock_drive();   
        }
        for(i = 4; i < 8; i++)
        {
              reset_value_UART_MCR_RTS();
              clock_drive();
       	}
	    delay();
       	       
}

void sequence_two()
{
	int i;
	reset_all();
	delay();
	for(i = 0; i < 4; i++)
	{
		set_value_UART_MCR_RTS();
        clock_drive();
		reset_value_UART_MCR_RTS();
        clock_drive();
	}
	delay();
}

void sequence_three(void)
{
	int i;
	reset_all();
	delay();
	for(i = 0; i < 8; i++)
	{
		set_value_UART_MCR_RTS();
        clock_drive();
	}
	delay();
}


ssize_t led_write(struct file *fd, const char __user *buf, size_t len, loff_t *off)
{
	int ret_val, i = 0, num_repetitions, j;
	char single_char;

	ret_val = copy_from_user(&single_char, &buf[i], 1);
	if(single_char == '1')
	{
	        i = i + 1;
	        ret_val = copy_from_user(&single_char, &buf[i], 1);
	        if(single_char >= 48 && single_char <= 57)
	    	{
                	num_repetitions = single_char - '0';
			        for(j = 0; j < num_repetitions; j++)	
			        {
				          sequence_one();			
                    }
                    return len;
            }
	    	else 
			        return len;
	    
	}
	else if(single_char == '2')
	{
	    	i = i + 1;
            ret_val = copy_from_user(&single_char, &buf[i], 1);
            if(single_char >= 48 && single_char <= 57)
            {
                    num_repetitions = single_char - '0';
                    for(j = 0; j < num_repetitions; j++)
                    {
                            sequence_two();
                    }
			        return len;
           }
           else
                    return len;

	}
	else if(single_char == '3')
	{
           i = i + 1;
           ret_val = copy_from_user(&single_char, &buf[i], 1);
           if(single_char >= 48 && single_char <= 57)
           {
                   num_repetitions = single_char - '0';
                   for(j = 0; j < num_repetitions; j++)
                   {
                           sequence_three();
                   }
		           return len;
           }
           else
                   return len;

	}
	else
	    return len;
}

static int led_init(void)
{
	fops.write = led_write;
	if(((major = register_chrdev(major, "leddriver", &fops) == -1)))
		return -1;
	return 0;
}

static void led_exit(void)
{
	unregister_chrdev(major, "leddriver");
}

module_init(led_init);
module_exit(led_exit);
