/* Application program for the LED Circuit to glow all LEDs simultaneously */

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
	unsigned char port_value;
	unsigned int data;
	const unsigned char mask = 0x01;
	int i;
	
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
		printf("\nEnter the Data to glow the LEDs : ");
		scanf("%x", &data);
		while(getchar() != '\n');				//Clear the input buffer
		
		port_value = inb(SERIAL_PORT + UART_MCR);
		
		for(i = 0; i < 8; i++)
		{
			if((data >> i) & mask)
				port_value |= UART_MCR_RTS;		//Glow the LED
			else
				port_value &= (~UART_MCR_RTS);	//reset the LED
			
			//Clock
			port_value |= UART_MCR_DTR;
			outb(port_value, SERIAL_PORT + UART_MCR);
			port_value = inb(SERIAL_PORT + UART_MCR);
			port_value &= (~UART_MCR_DTR);
			outb(port_value, SERIAL_PORT + UART_MCR);
			
		}
	}

	return 0;
}
