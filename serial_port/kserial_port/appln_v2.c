/* A simple application for file operations for explaining the minor no. concept*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define SIZE 20
#define FMODE O_WRONLY

int main()
{
	unsigned int choice;
	char buf[SIZE], source[100];//, mode[25];
	int wd;
	size_t bytes = 1;

	while(1)
	{
		choice = 100;	//any value other than required
		printf("\n1 : Open a file\
				\n2 : Exit the program\n");

		printf("\nEnter your choice : ");
		scanf("%u", &choice);
		while(getchar() != '\n');		//clear the input buffer

		switch(choice)
		{
			case 1 : printf("\nEnter the file name to to opened : ");
					 scanf("%s", source);
					 while(getchar() != '\n');	//clear the input buffer
					
					 wd = open(source, FMODE);
					 if(wd == -1)
					 {
						puts("Error: cannot open source file\n");
						exit(1);
					 }
					 write(wd, buf, bytes);
					 close(wd);
					 break;

			case 2 : return 0;

			default : printf("Enter a valid a valid digit\n");
		}
	}
}
