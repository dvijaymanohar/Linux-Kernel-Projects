#include <stdio.h>

static int test[4] = {0x0a, 0x09, 0x05, 0x06};

void print(int *ptr)
{
	printf("%d\n", *ptr);
}

int main()
{
	int j = 0;
	printf("%d--->%d\t%d\t%d\n", &test[j], *(test + j), test[2], test[3]);
	print(&test[j]);

	return 0;
}


