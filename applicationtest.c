#include <sys/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/serial_reg.h>

#define BASE_SERIAL_PORT 0x3f8
#define MODEM_REGISTER 6
//VMD

void set_value_UART_MSR_DSR()
{
	unsigned char value;
	value = inb(BASE_SERIAL_PORT + UART_MSR);
	value = value | UART_MSR_DSR;
	printf("%02x\n", value);
	outb(value, BASE_SERIAL_PORT + UART_MSR);
	value = inb(BASE_SERIAL_PORT + UART_MSR);
	printf("%02x\n" , value);
	
}

void reset_value_UART_MSR_DSR()
{
	unsigned char value;
	value = inb(BASE_SERIAL_PORT + UART_MSR);
	value = value & ~UART_MSR_DSR;
	outb(value, BASE_SERIAL_PORT + UART_MSR);
	value = inb(BASE_SERIAL_PORT + UART_MSR);
	printf("%02x\n" , value);
}
void set_value_UART_MSR_CTS()
{
	unsigned char value;
	value = inb(BASE_SERIAL_PORT + UART_MSR);
	value = value | UART_MSR_CTS;
	printf("%02x\n", value);
	outb(value, BASE_SERIAL_PORT + UART_MSR);
	value = inb(BASE_SERIAL_PORT + UART_MSR);
	printf("%02x\n" , value);
}
void reset_value_UART_MSR_CTS()
{
	unsigned char value;
	value = inb(BASE_SERIAL_PORT + UART_MSR);
	value = value & ~UART_MSR_CTS;
	outb(value, BASE_SERIAL_PORT + UART_MSR);
	value = inb(BASE_SERIAL_PORT + UART_MSR);
	printf("%02x\n" , value);
}

int main()
{
	int i, choice, choice1;
	unsigned char rd;

	if(ioperm(BASE_SERIAL_PORT, 8, 1))
		printf("No Permission\n");
	while(1)
	{
		printf("\n1)Read from Modem Control Register\n2)Set or Reset value of DSR and CTS in Modem Control Register\n3)Read from Line Status register\n4)Write to line Status Register\n5)Exit\n");
		printf("\nPlease enter choice: ");
		scanf("%d" , &choice);
		switch(choice)
		{
			case 1: rd = inb(BASE_SERIAL_PORT + UART_MSR);
					printf("%02x\n" , rd);
					break;
			case 2: printf("\n1)Set DSR\n2)Reset DSR\n3)Set CTS\n4)Reset CTS\n");
					printf("\nPlease enter choice: ");
					scanf("%d" , &choice1);
					switch(choice1)
					{
						case 1: set_value_UART_MSR_DSR();
								break;
						case 2: reset_value_UART_MSR_DSR();
								break;
						case 3: set_value_UART_MSR_CTS();
								break;
						case 4: reset_value_UART_MSR_CTS();
								break;
						default: break;
					}
					break;
					
			case 3: rd = inb(BASE_SERIAL_PORT + UART_LSR);
					printf("%02x\n" , rd);
					break;

		    case 4: break;	
			case 5: return 0;
		}
	}
}

