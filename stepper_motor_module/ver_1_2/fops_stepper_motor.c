#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define devfile "./stepper_drv"
#define FLAGS O_RDWR
#define count 8

void title(void);

#if 1
#define SIZE 4
//static int pattern[SIZE] = {0x0A, 0x09, 0x05, 0x06};
static int pattern[SIZE] = {0x05, 0x09, 0x0A, 0x06};
#else
#define SIZE 8
static int pattern[SIZE] = {0x0A, 0x08, 0x09, 0x01, 0x05, 0x04, 0x06, 0x02};
#endif

int main()
{
	int ret, handle, i, j = 0;
	unsigned char chr, reset = 0x00;

	title();
	if((handle = open(devfile, FLAGS)) == -1)
	{
		printf("Error: File cannot be opened!\n");
		return 0;
	}
	while(1)
	{
		printf("\n*~* Hit ENTER to proceed *~*\n");
		while((getchar()) != '\n');
		while(1)
		{
			do
			{
				printf("The Encrypted byte for %d step is  %x\n", j, *(pattern + j));
				if((ret = write(handle, &pattern[j], count)) == -1)
				{
					printf("Write Failed on the Device.\n");
					break;
				}	
				j++;
				for(i = 0; i < 199999999; i++);
			}while(j < SIZE);
	/*		if((ret = write(handle, &reset, count)) == -1)
			{
				printf("Write Failed on the Device.\n");
				break;
			}*/
			j = 0;
	/*		j = SIZE - 1;
			do
			{
				printf("The Encrypted byte for anti-clockwise direction is %x\n", *(pattern + j));
				if((ret = write(handle, &pattern[j], count)) == -1)
				{
					printf("Write Failed on the Device.\n");
					break;
				}	
				j--;
				for(i = 0; i < 199999999; i++);
			}while(j >= 0);
			if((ret = write(handle, &reset, count)) == -1)
			{
				printf("Write Failed on the Device.\n");
				break;
			}*/
		}
		printf("\n\t*~* Enjoy Buddy!! *~*\n\t(Hit ENTER to proceed)\n");
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
			j = 0;
		}while(chr == '\n');
	}
	return 0;
}


void title(void)
{
	int i;

	for(i = 0; i <= 85; i++)
		printf("*");
	printf("\n");
	printf("\t\t:|: Welcome to Stepper Motor control Module Application :|:\t\t\n");
	for(i = 0; i <= 85; i++)
		printf("*");
	printf("\n\n");
/*	printf("Loading\n");
	for(i = 1; i <= 8; i++)
	{
		for(j = 0; j < i; j++)
			printf("*");
		system("sleep 1");
		printf("\n\b");
	}*/
}

