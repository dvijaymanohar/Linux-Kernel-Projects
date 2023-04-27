# include <stdio.h>
# include <sys/stat.h>
# include <fcntl.h>

# define PATTERN1 0xAA
# define PATTERN2 0x55
# define PATTERN3 0xFF
# define PATTERN4 0x00
# define PATTERN5 0x80
# define PATTERN6 0x66
# define PATTERN7 0x81
# define PATTERN8 0x7E


void runastrain(char totalpattern, int openserial);
void delay(void);

int main()
{
	int input, run;
	unsigned char inputchar, inputchartemp;
	int openserial, error;
	printf("Enter the pattern(! to exit)\n");
	openserial = open("/dev/readserial", O_RDWR);
	if(openserial == -1)
		printf("Eror getting Access\n");
	else
		printf("Access Granted\n");
	printf("Enter the option. (Input as two digit Hexa).\n First three bits pattern.\nNext bit direction.\nLasr four bits number of times.\n");
	printf("Pattern.\n\t000 - 10101010.\n\t001 - 01010101.\n\t010 - 11111111.\n\t011 - 00000000.\n\t");
	printf("100 - 10011001.\n\t101 - 01100110.\n\t110 - 10000001.\n\t111 - 01111110.\n");
	printf("Direction.\n\t0 - left to right.\n\t1 - right to left.\n");
	printf("Number of times.\n\t0000 - 0 times to 1111 - 16 times\n");
	scanf("%x", &input);
	inputchar = input;
	runastrain(inputchar, openserial);
	return 0;
}

void runastrain(char totalpattern, int openserial)
{
	int number, error;
	unsigned char pattern, direction, numberoftimes, temppattern;
	pattern = totalpattern & 0xE0;
	pattern = pattern >> 5;
	if(pattern == 0x07)
		pattern = PATTERN8;
	else if(pattern == 0x06)
		pattern = PATTERN7;
	else if(pattern == 0x05)
		pattern = PATTERN6;
	else if(pattern == 0x04)
		pattern = PATTERN5;
	else if(pattern == 0x03)
		pattern = PATTERN4;
	else if(pattern == 0x02)
		pattern = PATTERN3;
	else if(pattern == 0x01)
		pattern = PATTERN2;
	else if(pattern == 0x00)
		pattern = PATTERN1;
	direction = totalpattern & 0x10;
	direction = direction >> 4;
	numberoftimes = totalpattern & 0x0F;
	temppattern = pattern;
	for(number = 0x00; number < numberoftimes; number++)
	{
		if(temppattern == 0x00)
		{
			temppattern = pattern;
			printf("over\n");
		}
		error = write(openserial, &temppattern, 1, 0);
		if(direction == 0x00)
		{
			temppattern = temppattern >> 1;
			printf("Left to right%x\n", temppattern);
		}
		else if(direction == 0x01)
		{
			temppattern = temppattern << 1;
			printf("Right to left%x\n", temppattern);
		}
		delay();
	}
}

void delay(void)
{
	unsigned long i;
	for(i = 0; i < 199999999; i++);
}

