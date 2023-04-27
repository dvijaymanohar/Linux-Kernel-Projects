#include <stdio.h>

int main()
{
	unsigned int input, count, bits = 0;
	char mask = 0x01;

	printf("Enter the number: ");
	scanf("%u", &input);
	count = 8 * sizeof(int);
	(input & mask) ? bits++ : bits + 0;
	do
	{
		((input >>= 1) & mask) ? bits++ : bits + 0;
	}while (input != 0);
	printf("\nThe number of bits set in the input number is/are: %d\n", bits);
	return 0;
}
