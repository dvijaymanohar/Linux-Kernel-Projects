#include <stdio.h>
#include <sys/io.h>
#include <linux/serial_reg.h>
#include "modified_uart.h"

void title(void);

int main()
{
	int option = 10, ret;

	title();
	if((ret = ioperm(BASE_ADDR, 8, 1)) == -1)
	{
		printf("Oh Crap! You've no Permission!\n");
		return 0;
	}
	while(1)
	{
		printf("Enter your choice:\
				\n1. For sending Data 1(High).\n2.For sending Data 0(Low) 3. Exit: ");
		scanf("%d", &option);
		switch(option)
		{
			case 1:
				set_bit(~UART_MCR_RTS, BASE_ADDR + UART_MCR);
				set_bit(~UART_MCR_DTR, BASE_ADDR + UART_MCR);
				reset_bit(UART_MCR_DTR, BASE_ADDR + UART_MCR);
				break;
			case 2:
				reset_bit(UART_MCR_RTS, BASE_ADDR + UART_MCR);
				set_bit(~UART_MCR_DTR, BASE_ADDR + UART_MCR);
				reset_bit(UART_MCR_DTR, BASE_ADDR + UART_MCR);
				break;
			case 3:
				printf("\n\t~: Thanks! :~\n");
				return 0;
			default:
				printf("Invalid input!\n");
		}
	}
}

void reset_bit(unsigned char bit, unsigned port)
{
	unsigned receive = 0, test = 0;
	
	receive = inb(port);
	receive = receive | bit;
	printf("\n-->> 0x%02X", receive);
	write_reg(receive, port);
	test = inb(port);
	printf("\nThe Value on the Register: 0x%02X\n", test);
}

void set_bit(unsigned char mask, unsigned port)
{
	unsigned receive = 0, test = 0;
	
	receive = inb(port);
	receive = (receive & mask);
	printf("\n-->> 0x%02X", receive);
	write_reg(receive, port);
	test = inb(port);
	printf("\nWritten Value on the Register: 0x%02X\n", test);
}

void title(void)
{
	int i;

	for(i = 0; i <= 75; i++)
		printf("*");
	printf("\n");
	printf("\t\tThis is Test Application for Module\t\t\n");
	for(i = 0; i <= 75; i++)
		printf("*");
	printf("\n\n");
}

