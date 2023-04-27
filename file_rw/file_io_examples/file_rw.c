/* Program to clear the concept of file operations */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
//#include <types.h>
//#include <stat.h>

#define SIZE 32
#define NMEMB 1

int main()
{
	FILE *rd, *wd;
	unsigned int choise;
	char buffer[SIZE];

	rd = fopen("source", "rb+");
	if(rd == NULL)
	{
		puts("Error: cannot open source file\n");
		exit(1);
	}

	wd = fopen("target", "wb+");
	if(rd == NULL)
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

			case 2 : write_file(buffer, wd);
				 break;

			case 3 : fclose(rd);
				 fclose(wd);
				 return 0;

			default : printf("Enter a valid a valid digit\n");
		}
	}
}


void read_file(char *buffer, FILE *rd)
{
	memset(buffer, 0, SIZE);	

	while(!feof(rd))
	{
		fread(buffer, SIZE, NMEMB, rd);
	 	printf("%s", buffer);	
	}
}


void write_file(char *buffer, FILE *wd)
{
	memset(buffer, 0, SIZE);	

	printf("Enter the data to be written to the file :\n");
	while(1)
	{
		scanf("%s", buffer);
		fwrite(buffer, SIZE, NMEMB, wd);
		while(getchar() != '\n');	//clear the input buffer
		printf("Add another line (y/n) : ");
		if(getchar() != 'y')
			break;
		while(getchar() != '\n');	//clear the input buffer
	}
}
