#include <stdio.h>
#include <sys/io.h>
#include <ctype.h>
#include <linux/serial_reg.h>
#include "uart_io.h"

void lcr_ops(unsigned char DLAB, unsigned port)
{
	char choice = 'A';
	int optin = 0;
	unsigned receive = 0, test = 0;
	
	while(1)
	{
		printf("\nEnter your choice for the operation:\
						\n'R'- Read from LCR Port 'W' - Write to LCR Port\
						\n'E'- Exit to Main menu : ");
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
						\n1.Set SBC\n2. Reset SBC\n3.Set Parity Enable\n4.Reset Parity Enable\n5. Go Back: ");
				scanf("%d", &optin);
				while((getchar()) != '\n');
				switch(optin)
				{
					case 1:
						set_bit(LCR_SBC_MASK, port);
						break;
					case 2:
						reset_bit(LCR_SBC_MASK, UART_LCR_SBC, port);
						break;
					case 3:
						set_bit(LCR_PARITY_MASK, port);
						break;
					case 4:
						reset_bit(LCR_PARITY_MASK, UART_LCR_PARITY, port);
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

