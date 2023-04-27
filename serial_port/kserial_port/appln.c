/* A simple application for file operations */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define SIZE 20
#define FMODE O_WRONLY

int main()
{
	unsigned int choice, led_data, i;
	char buf[SIZE], source[100];//, mode[25];
	int wd;
	size_t bytes;

	printf("\nEnter the file name to to opened : ");
	scanf("%s", source);
	while(getchar() != '\n');	//clear the input buffer
	
/*	printf("\nEnter the mode in which %s is to be opened : ", source);
	scanf("%s", mode);
	while(getchar() != '\n');	//clear the input buffer
*/		
	wd = open(source, FMODE);
	if(wd == -1)
	{
		puts("Error: cannot open source file\n");
		exit(1);
	}

	while(1)
	{
		choice = 100;	//any value other than required
		printf("\n1 : Write to the file\
				\n2 : Close the file and exit the program\n");

		printf("\nEnter your choice : ");
		scanf("%u", &choice);
		while(getchar() != '\n');		//clear the input buffer

		switch(choice)
		{
			case 1 : printf("Enter the no. of bytes to be written : ");
					 scanf("%u", &bytes);
					 while(getchar() != '\n');

					 printf("Enter the data byte(s) to the LED circuit : ");
					 for(i = 0; i < bytes; i++)
					 {
						scanf("%x", &led_data);
						buf[i] = led_data;
					 }
					 //buf[i] = '/0';
					 while(getchar() != '\n');
					/*
					 if(bytes > strlen(buf))
					 	printf("Data to be written is more than the entered data\n");
					 else
					*/	write(wd, buf, bytes);
					 break;

			case 2 : close(wd);
					 return 0;

			default : printf("Enter a valid a valid digit\n");
		}
	}
}
