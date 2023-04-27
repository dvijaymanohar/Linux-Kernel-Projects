#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "serial_io.h"

void title(void);
void delay(void);
unsigned char select_pattern(unsigned char option);

int main()
{
	unsigned char repeat_count, move, chr, inp, mask = 0x00, push, counter, temp_var;
	unsigned int option, ret, handle, pattern;

	title();
	if((handle = open("./led_dev", FLAGS)) == -1)
	{
		printf("Error: File cannot be opened!\n");
		return 0;
	}
	while(1)
	{
		printf("Enter your choice:\
				\n~:~ Bitwise Option for the LED pattern: |000 - 10000000\t-\t001 - 10101010|\
				\n\t\t\t\t\t|010 - 00110011\t-\t011 - 00001111|\n\t\t\t\t\t|100 - 10011001\t-\t101 - 11000011|\
				\n\t\t\t\t\t|110 - 11111111\t-\t111 - 01111110|");
		printf("\n~:~ Bitwise Option for Directions:\t|0 - Right to Left ~ 1 - Left to Right|");
		printf("\n~:~ Bitwise Option for no. of\n  repeatations you wish for the pattern: ");
		printf("|0000 - 1 time\t ~through~\t1111 - 16 times|\n(Enter 81 to exit)\nYour option: ");
		scanf("%x", &option);
		if(option == 0x81)
		{
			printf("\n\t~:~ Thanks for using this application ~:~\n\t           # Amarsingh #\n");
			break;
		}
		temp_var = option;
		if((pattern = select_pattern(temp_var)) == -1)
		{
			printf("Shutting Down!\n\t~: Thanks :~\n");
			return 0;
		}
		inp = pattern;
		repeat_count = option & 0x0F;
		for(counter = 0; counter <= repeat_count; counter++)
		{
			for(move = 1; move <= LED_BITS; move++)
			{
				if((ret = write(handle, &inp, count)) == -1)
				{
					printf("Write Failed on the Device.\n");
					break;
				}
				if((temp_var & 0x10) == 0x10)
				{
					inp = (inp >> 1) | (mask << (8 - move));
					printf("\nPattern shifted to right..");
					for(push = 0; push < move; push++)
						printf("*");
				}
				else if((temp_var & 0x10) == 0x00)
				{
					inp = (inp << 1) | (mask >> (8 - move));
					printf("\nPattern shifted to left..");
					for(push = 0; push < move; push++)
						printf("*");
				}
				delay();
			}
			inp >>= 1;
			if((ret = write(handle, &inp, count)) == -1)
			{
				printf("Write Failed on the Device.\n");
				break;
			}
			printf("\n\tEnd of %d repeatation(s).", (counter + 1));
			if((counter % 2) != 0)
			{
				mask = pattern;
				inp = 0x00;
			}
			else if((counter % 2) == 0)
			{
				inp = pattern;
				mask = 0x00;
			}
		}
		printf("\nDo you wish to continue?(y or n): ");
		while((getchar()) != '\n');
		scanf("%c", &chr);
		if(chr != 'y' && chr != '\n')
		{
			printf("\n\t~:~ Thanks for using this application ~:~\n\t           # Amarsingh #\n");
			break;
		}
		mask = 0x00;
	}
	return 0;
}

unsigned char select_pattern(unsigned char option)
{
	unsigned char pattern;

	pattern = (option & 0xE0);
	pattern >>= 5;
	switch(pattern)
	{
		case 0x00:
				pattern = LED_PATTERN_0;
				break;
		case 0x01:
				pattern = LED_PATTERN_1;
				break;
		case 0x02:
				pattern = LED_PATTERN_2;
				break;
		case 0x03:
				pattern = LED_PATTERN_3;
				break;
		case 0x04:
				pattern = LED_PATTERN_4;
				break;
		case 0x05:
				pattern = LED_PATTERN_5;
				break;
		case 0x06:
				pattern = LED_PATTERN_6;
				break;
		case 0x07:
				pattern = LED_PATTERN_7;
				break;
		default:
				printf("Invalid input!.\n");
				return -1;
	}
	return pattern;
}

void delay(void)
{
	int i;
	
	for(i = 0; i <= 200000000; i++);
}

void title(void)
{
	int i;

	for(i = 0; i <= 75; i++)
		printf("*");
	printf("\n");
	printf("\t\t:|: Welcome to LED Display Module :|:\t\t\n");
	for(i = 0; i <= 75; i++)
		printf("*");
	printf("\n\n");
}

