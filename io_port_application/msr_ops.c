#include <stdio.h>
#include <sys/io.h>
#include <ctype.h>
#include <linux/serial_reg.h>
#include "uart_io.h"

void msr_ops(unsigned char DLAB, unsigned port)
{
	char choice = 'A';
	int optin = 0;
	unsigned receive = 0, test = 0;
	
	while(1)
	{
		printf("\nEnter your choice for the operation:\
						\n'R'- Read from MSR Port 'W' - Write to MSR Port\
						\n'E'- Exit to Main menu: ");
		scanf("%c", &choice);
		switch(tolower(choice))
		{
			case 'r':
				set_DLAB(DLAB);
				read_reg(port);
				break;
			case 'w':
				set_DLAB(DLAB);
				printf("\nChoose from the options:\
						\n1. Set DSR\n2. Reset DSR\n 3. Set CTS\n4. Reset CTS\n5. Go Back : ");
				while((getchar()) != '\n');
				scanf("%d", &optin);
				switch(optin)
				{
					case 1:
						set_bit(MSR_DSR_MASK, port);
						break;
					case 2:
						reset_bit(MSR_DSR_MASK, UART_MSR_DSR, port);
						break;
					case 3:
						set_bit(MSR_CTS_MASK, port);
						break;
					case 4:
						reset_bit(MSR_CTS_MASK, UART_MSR_CTS, port);
						break;
					case 5:
						break;
					default:
						printf("\nInvalid input!");
				}
				break;
			case 'e':
				return;
			default:
				printf("\nInvalid choice entered!\
						\nhit Enter to continue\n");
		}
		while((getchar()) != '\n');
	}
}
