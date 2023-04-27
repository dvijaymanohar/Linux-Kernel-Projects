#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define FLAG O_RDWR

int main ()
{
	int fd = -1, choice, count, rd, i;
	char *filepath;
	char *rbuf, *wbuf;

	if((filepath = (char *)malloc(50 * sizeof(char))) == NULL)
	{
		printf("Memory alocation failed.\n");
		return 0;
	}
	printf("\nEnter the filename(alongwith its Path): ");
	scanf("%s", filepath);
	if((fd = open(filepath, FLAG)) == -1)
	{
		printf("Error in opening the prescribed file!\n");
		return 0;
	}
	else
		printf("\nFile successfuly opened!\n");
	while(1)
	{
		printf("Enter Your choice:\n1. Read from the File.\
				\n2. Write into a File. 3. Close the opened File and exit.: ");
		scanf("%d", &choice);
		switch(choice)
		{
			case 1:
				while(getchar() != '\n');
				printf("Enter the number of bytes you wish to Read from the File: ");
				scanf("%d", &count);
				if((rbuf = malloc((count + 10) * sizeof(char))) == NULL)
				{
					printf("Memory allocation failed!\n");
					break;
				}
				if((rd = read(fd, rbuf, count)) == -1)
				{
					printf("Read operation failed!\n");
					break;
				}
				i = 0;
				rbuf[count] = '\0';
				while(rbuf[i] != '\0')
				{
					printf("%c", rbuf[i]);
					i++;
				}
				putchar('\n');
				free(rbuf);
				break;
			case 2:
				while(getchar() != '\n');
				if((wbuf = malloc(100 * sizeof(char))) == NULL)
				{
					printf("Memory allocation failed1\n");
					break;
				}
				printf("Enter the Data(put '~' at the end): ");
				scanf("%[^~]s", wbuf);
				while(getchar() != '\n');
				printf("Enter number of bytes you wish to Write onto the File: ");
				scanf("%d", &count);
				if(strlen(wbuf) < count)
				{
					printf("The Entered count is greater than the Data to be written!\n");
					break;
				}
				if((rd = write(fd, wbuf, count)) == -1)
				{
					printf("Write operation failed!\n");
					break;
				}
				else
					printf("Write operation successful.\n");
				free(wbuf);
				break;
			case 3:
				if(fd != -1)
				{
					if((close(fd)) == -1)
					{
						printf("Close operation failed!\n");
						return 0;
					}
					else
						printf("File closed successfully!\n");
					return 0;
				}
				else
					printf("No file is open!\n");
				break;
			default:
				printf("Wrong choice entered!\n");
				break;
		}
		while(getchar() != '\n');
	}
}
