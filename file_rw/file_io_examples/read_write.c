/* Program to clear the concept of file operations */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
//#include <types.h>
//#include <stat.h>

#define SIZE 512

void read_file(char *buffer, int rd);
void write_file(char *buffer, int wd, int rd);


int main()
{
	unsigned int choise;
	char buffer[SIZE];
	int rd, wd;


	rd = open("source", O_RDONLY);
	if(rd == -1)
	{
		puts("Error: cannot open source file\n");
		exit(1);
	}

	wd = open("target", O_WRONLY);
	if(rd == -1)
	{
		puts("Error: cannot open target file\n");
		exit(1);
	}

	while(1)
	{
		choise = 100;	//any value other than required
		printf("\n1 : Read from the file\
				\n2 : Write to a file\
				\n3 : Close the file and exit the program\n");

		printf("\nEnter your choise : ");
		scanf("%u", &choise);
		while(getchar() != '\n');		//clear the input buffer

		switch(choise)
		{
			case 1 : read_file(buffer, rd);
					 break; 

			case 2 : write_file(buffer, wd, rd);
					 break;

			case 3 : close(rd);
					 close(wd);
					 return 0;

			default : printf("Enter a valid a valid digit\n");
		}
	}
}


void read_file(char *buffer, int rd)
{
	int bytes;

	memset(buffer, 0, SIZE);	

	while(1)
	{
		bytes = read(rd, buffer, SIZE);
		if(bytes <= 0)
			break;
	 	printf("%s", buffer);
	}
}


void write_file(char *buffer, int wd, int rd)
{
	int bytes;

	memset(buffer, 0, SIZE);	

	while(1)
	{
		bytes = read(rd, buffer, SIZE);
		if(bytes > 0)
			write(wd, buffer, bytes);
		else
			break;
	}
}
