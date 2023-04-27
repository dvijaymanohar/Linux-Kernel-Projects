/* Application program for controlling the Serial Port(pin 4 & 7) */

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

	while(1)
	{
		choice = main_menu();
		do
		{
			switch(choice)
			{
				case 1 : serial_port_bit = UART_MCR_DTR;
						 break;

				case 2 : serial_port_bit = UART_MCR_RTS;
						 break;

				case 3 : exit(1);
						 break;

				default : printf("Error: Enter a valid a valid digit : ");
						  scanf("%u", &choice); 
						  while(getchar() != '\n');
			}
		}while(choice < 1 || choice >3);
		
		choice = submenu();
		do
		{
			switch(choice)
			{
				case 1 : port_value = inb(SERIAL_PORT + UART_MSR);
						 port_value &= (~serial_port_bit);
						 outb(port_value, SERIAL_PORT + UART_MCR);
						 break;

				case 2 : port_value = inb(SERIAL_PORT + UART_MSR);
						 port_value |= serial_port_bit;
						 outb(port_value, SERIAL_PORT + UART_MCR);
						 break;

				case 3 : break;

				default : printf("Error: Enter a valid a valid digit : ");
						  scanf("%u", &choice);
						  while(getchar() != '\n');
			}
		}while(choice < 1 || choice >3);
	}

	return 0;
}



int main_menu(void)
{
	int	choice = 100;	//any value other than required

	printf("\n1 : DTR(Pin 4)\
			\n2 : RTS(Pin 7)\
			\n3 : Exit the application\n");

	printf("Enter your choice : ");
	scanf("%u", &choice);
	while(getchar() != '\n');		//clear the input buffer

	return(choice);
}


int submenu(void)
{
	int	choice = 100;	//any value other than required

	printf("\n1 : Set the bit\
			\n2 : Reset the bit\
			\n3 : Go to main menu\n");

	printf("Enter your choice : ");
	scanf("%u", &choice);
	while(getchar() != '\n');		//clear the input buffer

	return(choice);
}
