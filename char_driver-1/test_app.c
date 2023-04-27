#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#define TEST_DEV	"/dev/exp_dev"
int main()
{
	int fd;
	unsigned char r_buf[20] = { '\0' };
	unsigned char w_buf[20] = { "Hello World\n" };
	int retval;

	fd = open(TEST_DEV,O_RDWR);
	if(fd < 0){
		printf("Unable to open device\n");
		return -1;
	}

	retval = write(fd, &w_buf[0], 20);
	printf("return value is %d\n", retval);

	lseek(fd, 0, SEEK_SET);

	retval = read(fd, &r_buf[0], 20);
	printf("return value is %d\n", retval);
	printf("read value is %s\n",r_buf);

	close(fd);
	return retval;
}

