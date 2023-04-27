/* Application program for the LED Circuit to glow LEDs one by one */

#include <stdio.h>
#include <stdlib.h>
#include <sys/io.h>
#include <linux/serial_reg.h>

#define SERIAL_PORT 0x03F8
#define RANGE 8
#define TURN_ON 1

int main_menu(void);

int main()
{
	unsigned int choice, port_value, serial_port_bit;
	
	if(ioperm(SERIAL_PORT, RANGE, TURN_ON))
	{
		printf("Insufficient privilege to call ioperm()\n");
		exit(1);
	}

	port_value = inb(SERIAL_PORT + UART_MCR);
	port_value &= 0xEF;
	outb(port_value, SERIAL_PORT + UART_MCR);
	
	while(1)	
	{
		choice = submenu();
		
		switch(choice)
		{
			case 1 : port_value = inb(SERIAL_PORT + UART_MCR);
					 port_value |= UART_MCR_RTS;
					 port_value |= UART_MCR_DTR;
					 outb(port_value, SERIAL_PORT + UART_MCR);
					 port_value = inb(SERIAL_PORT + UART_MCR);
					 port_value &= (~UART_MCR_DTR);
					 outb(port_value, SERIAL_PORT + UART_MCR);
					 break;
			
			case 2 : port_value = inb(SERIAL_PORT + UART_MCR);
					 port_value &= (~UART_MCR_RTS);
					 port_value |= UART_MCR_DTR;
					 outb(port_value, SERIAL_PORT + UART_MCR);
					 port_value = inb(SERIAL_PORT + UART_MCR);
					 port_value &= (~UART_MCR_DTR);
					 outb(port_value, SERIAL_PORT + UART_MCR);
					 break;
					 
			case 3 : exit(1);

			default : printf("Error: Enter a valid a valid digit");
		}
	}

	return 0;
}



int submenu(void)
{
	int	choice = 100;

	while(choice < 1 || choice > 3)
	{
		choice = 100;	//any value other than required
		printf("\n1 : Glow the LED\
				\n2 : Turn off the LED\
				\n3 : Exit the application\n");

		printf("Enter your choice : ");
		scanf("%u", &choice);
		while(getchar() != '\n');		//clear the input buffer
	}
	
	return(choice);
}
