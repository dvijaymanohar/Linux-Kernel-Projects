#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define devfile "./led_drv"
#define FLAGS O_RDWR
#define count 8

void title(void);
unsigned int select_pattern(unsigned int);
unsigned char direction(unsigned int);

int main()
{
	int option = 10, ret, handle;
	unsigned int return_value, pattern_option, direct_option = 0x00, repeat_option;
	unsigned char pattern, temp_var, chr;

	title();
	if((handle = open(devfile, FLAGS)) == -1)
	{
		printf("Error: File cannot be opened!\n");
		return 0;
	}
	while(1)
	{
		printf("Enter your choice:\
				\n~:~ Options for the LED patterns: |0 - 10000000\t-\t1 - 10101010|\
				\n\t\t\t\t|2 - 00110011\t-\t3 - 00001111|\n\t\t\t\t|4 - 11100111\t-\t5 - 10011001|\
				\n\t\t\t\t|6 - 11111111\t-\t7 - 01111110|\n -$ Your Option: ");
		scanf("%u", &pattern_option);
		if((return_value = select_pattern(pattern_option)) == -1)
		{
			printf("Invalid Pattern option!\n");
			continue;
		}
		do
		{
			printf("\n~:~ Options for Directions:\t|0 - Right to Left ~ 1 - Left to Right|\n -$ Your option: ");
			scanf("%u", &direct_option);
			if((return_value = direction(direct_option)) == -1)
			{
				printf("Invalid Direction option!\n");
				continue;
			}
		}while(return_value == -1 || direct_option > 1);
		do
		{
			printf("\n~:~ Option for no. of\n  repeatations you wish for the pattern: ");
			printf("|0 for 1 time\t ~through~\t15 for 16  times|\n(Enter 81 to exit)\n -$ Your option: ");
			while((getchar()) != '\n');
			scanf("%u", &repeat_option);
			if(option == 81)
			{
				printf("\n\t~:~ Thanks for using this application ~:~\n\t\t    # Amarsingh #\n");
				return 0;
			}
			else if(repeat_option > 15)
			{
				printf("\nInvalid option!\n");
				continue;
			}
			else
				printf("You've selected %u repeatations for your pattern.\n", (repeat_option + 1));
		}while(repeat_option > 15);
		pattern = pattern_option;
		pattern <<= 5;
		temp_var = direct_option;
		pattern |= ((temp_var << 4) & 0x10);
		temp_var = repeat_option;
		pattern |= (temp_var & 0x0F);
		printf("The Encrypted byte is %x\n", pattern);
		if((ret = write(handle, &pattern, count)) == -1)
		{
			printf("Write Failed on the Device.\n");
			break;
		}
		printf("\t*~* Enjoy the pattern Buddy!! *~*\n");
		system("sleep 1");
		while((getchar()) != '\n');
		do
		{
			printf("\nDo you wish to continue?(y or n): ");
			scanf("%c", &chr);
			if(chr != 'y' && chr != '\n')
			{
				printf("\n\t~:~ Thanks for using this application ~:~\n\t\t    # Amarsingh #\n");
				return 0;
			}
		}while(chr == '\n');
			pattern_option = repeat_option = 99;
	}
	return 0;
}

unsigned int select_pattern(unsigned int option)
{

	switch(option)
	{
		case 0:
				printf("Pattern 0 - |>| 10000000 |<| has been selected.\n");
				break;
		case 1:
				printf("Pattern 1 - |>| 10101010 |<| has been selected.\n");
				break;
		case 2:
				printf("Pattern 2 - |>| 00110011 |<| has been selected.\n");
				break;
		case 3:
				printf("Pattern 3 - |>| 00001111 |<| has been selected.\n");
				break;
		case 4:
				printf("Pattern 4 - |>| 11100111 |<| has been selected.\n");
				break;
		case 5:
				printf("Pattern 5 - |>| 10011001 |<| has been selected.\n");
				break;
		case 6:
				printf("Pattern 6 - |>| 11111111 |<| has been selected.\n");
				break;
		case 7:
				printf("Pattern 7 - |>| 01111110 |<| has been selected.\n");
				break;
		default:
				return -1;
	}
	return 0;
}

unsigned char direction(unsigned int option)
{
	if(option == 0)
		printf("Note : Right-to-Left direction is selected for the pattern movement.\n");
	else if(option == 1)
		printf("Note : Left-to-Right direction is selected for the pattern movement.\n");
	else
		return -1;
	return 0;
}

void title(void)
{
	int i;

	for(i = 0; i <= 75; i++)
		printf("*");
	printf("\n");
	printf("\t\t:|: Welcome to LED Module Application :|:\t\t\n");
	for(i = 0; i <= 75; i++)
		printf("*");
	printf("\n\n");
/*	printf("Loading\n ");
	for(i = 1; i <= 8; i++)
	{
		for(j = 0; j < i; j++)
			printf("*");
		system("sleep 1");
		printf("\n\b");
	}*/
}

