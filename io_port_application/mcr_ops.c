#include <stdio.h>
#include <sys/io.h>
#include <ctype.h>
#include <linux/serial_reg.h>
#include "uart_io.h"

void mcr_ops(unsigned char DLAB, unsigned port)
{
	char choice = 'A';
	int optin = 0;
	unsigned receive = 0, test = 0;
	
	while(1)
	{
		printf("\nEnter your choice for the operation:\
						\n'R'- Read from MCR Port 'W' - Write to MCR Port\
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
						\n1.Set LoopBack Test Mode\n2. Reset LoopBack Test Mode\n3.Set RTS bit\n4.Reset RTS bit\
						\n5. Set DTR\n6. Reset DTR \n7. Go Back :  ");
				while((getchar()) != '\n');
				scanf("%d", &optin);
				switch(optin)
				{
					case 1:
						set_bit(MCR_LOOP_MASK, port);
						break;
					case 2:
						reset_bit(MCR_LOOP_MASK, UART_MCR_LOOP, port);
						break;
					case 3:
						set_bit(MCR_RTS_MASK, port);
						break;
					case 4:
						reset_bit(MCR_RTS_MASK, UART_MCR_RTS, port);
						break;
					case 5:
						set_bit(MCR_DTR_MASK, port);
						break;
					case 6:
						reset_bit(MCR_DTR_MASK, UART_MCR_DTR, port);
						break;
					case 7:
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

