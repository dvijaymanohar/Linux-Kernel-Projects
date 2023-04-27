#include <stdio.h>
#include <sys/io.h>
#include <ctype.h>
#include <linux/serial_reg.h>
#include "uart_io.h"

void title(void);

int main()
{
	int opt = 0, ret = 1;
	unsigned value = 0;
	unsigned char DLAB = 0;
	
	title();
	if((ret = ioperm(BASE_ADDR, 8, 1)) == -1)
	{
		printf("Oh Crap! You've no Permission!\n");
		return 0;
	}
	while(1)
	{
	/*	printf("\n\nEnter your choice for register:\
				\n|1. Receiver Buffer Register(Read only) || 2. Tx Holding Register(write only)|\
				\n|3. Interupt Enable Register || 4. Interrupt Identification Register(Read Only)|\
				\n|5. FIFO Control Register(Write Only) || 6. Line Control Register|\
				\n|7. MODEM Control Register || 8.Line Status Register|\
				\n|9. MODEM status Register || 10. Scratch Register|\
				\n|11. Divisor Latch(LS) || 12. Divisor Latch(MS)\t|| 13. Exit : ");*/
		printf("\n\nEnter your  choice for Register:\
				\n|1. Receiver Buffer Register(Read only) || 2. Tx Holding Register(Write only) |\
				\n|3. Line Control Register|| 4. MODEM Control Register \
				\n5. MODEM Status Register|| 6. Exit : ");
		scanf("%d", &opt);
		while((getchar()) != '\n');
		if(opt == 0)
		{
			printf("Invalid Input: Please Enter valid numeric option.\n");
			continue;
		}
		switch(opt)
		{
			case 1:
				DLAB= 0;
				set_DLAB(DLAB);
				printf("\nNote: This is Read Only Register.");
				read_reg(BASE_ADDR + UART_RX);
				break;
			case 2:
				DLAB = 0;
				set_DLAB(DLAB);
				printf("Enter 8-bit data you want to send to the port: ");
				scanf("%u", &value);
				write(value, (BASE_ADDR + UART_TX));
				break;
			case 3:
				lcr_ops(DLAB, (BASE_ADDR + UART_LCR));
				break;
			case 4:
				mcr_ops(DLAB, (BASE_ADDR + UART_MCR));
				break;
			case 5:
				msr_ops(DLAB, (BASE_ADDR + UART_MSR));
				break;
			case 6:
				return 0;
			default:
				printf("Excusez Moi, Monssier!, its a wrong choice!\n");
		}
		while((getchar()) != '\n');
	}
}


void reset_bit(unsigned char mask, unsigned char bit, unsigned port)
{
	unsigned receive = 0, test = 0;
	
	receive = inb(port);
	receive = (receive & mask) | bit;
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

void read_reg(unsigned port)
{
	unsigned receive = 0;
	receive = inb(port);
	printf("\nValue at Register is : 0x%02x\n", receive);
}
/*
void write_reg(unsigned value, unsigned port)
{
	outb(value, port);
}
*/
void set_DLAB(unsigned char bit)
{
	unsigned char recv = 0;

	recv = inb(BASE_ADDR + UART_LCR);
	recv = (recv & 0x7F) | bit;
	outb(recv, (BASE_ADDR + UART_LCR));
}

void title(void)
{
	int i;

	for(i = 0; i <= 80; i++)
		printf("*");
	printf("\n");
	printf("\t\tSimple UART Access Application\t\t\n");
	for(i = 0; i <= 80; i++)
		printf("*");
	printf("\n\n");
}
	
