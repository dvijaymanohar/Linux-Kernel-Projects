#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main ()
{
	FILE *fd_text;
	int fd_dev, count = 1;
	char read_buf;

	fd_text = fopen("./Readme.txt", "r");
	if((fd_dev = open("/dev/scull", O_RDWR)) == -1)
	{
		printf("File opening Failed\n");
		return 0;
	}
	
	fread(&read_buf, 1, 1, fd_text);
	do
	{
		write(fd_dev, &read_buf, count);
		fread(&read_buf, 1, 1, fd_text);
	}while(!(feof(fd_text)));
	
	fclose(fd_text);
	return 0;
}
