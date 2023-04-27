/* A simple application for file operations */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

#define SIZE 512
#define FMODE O_RDWR

int main()
{
	unsigned int choice;
	char buf[SIZE], source[100];//, mode[25];
	int rd;
	size_t bytes;

	printf("\nEnter the file name to to opened : ");
	scanf("%s", source);
	while(getchar() != '\n');	//clear the input buffer
	
/*	printf("\nEnter the mode in which %s is to be opened : ", source);
	scanf("%s", mode);
	while(getchar() != '\n');	//clear the input buffer
*/		
	rd = open(source, FMODE);
	if(rd == -1)
	{
		puts("Error: cannot open source file\n");
		exit(1);
	}

	while(1)
	{
		choice = 100;	//any value other than required
		printf("\n1 : Read from the file\
				\n2 : Write to a file\
				\n3 : Close the file and exit the program\n");

		printf("\nEnter your choice : ");
		scanf("%u", &choice);
		while(getchar() != '\n');		//clear the input buffer

		switch(choice)
		{
			case 1 : printf("Enter the no. of bytes to read : ");
					 scanf("%u", &bytes);
					 while(getchar() != '\n');
					 read(rd, buf, bytes);
					 buf[bytes] = '\0';		//######
					 printf("\n%s\n", buf);	//######for testing
					 break;

			case 2 : printf("Enter the data to write (termination char : |) : \n");
					 scanf("%[^|]s", buf);
					 while(getchar() != '\n');
					
					 printf("Enter the no. of bytes to be written : ");
					 scanf("%u", &bytes);
					 while(getchar() != '\n');

					 if(bytes > strlen(buf))
					 	printf("Data to be written is more than the entered data\n");
					 else
						write(rd, buf, bytes);
					 break;

			case 3 : close(rd);
					 return 0;

			default : printf("Enter a valid a valid digit\n");
		}
	}
}
