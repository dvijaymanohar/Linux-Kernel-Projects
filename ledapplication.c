#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define MAXSIZE 100

int main()
{
	int fd, val, choice;
	ssize_t value;
	char buf[MAXSIZE];
	fd = open("/dev/ledwrite" , O_RDWR);
	printf("fd is %d\n" , fd);
	while(1)
	{
		printf("1)Write sequence\n2)Exit\n");
		printf("Please enter choice\n");
		scanf("%d" , &choice);
		switch(choice)
		{
			case 1: printf("Enter sequence such that 1st digit = pattern num AND 2nd digit = num of reps\n");
				scanf("%s", buf);
				printf("Buf is %s\n" , buf);
				val = write(fd, buf, strlen(buf) + 1);
				val == -1 ? printf("Cannot write\n") : printf("written to file\n");
				break;
			case 2: return 0;
		}
	}
    return 0;
}
