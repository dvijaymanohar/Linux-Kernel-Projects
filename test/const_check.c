#include <stdio.h>

int main()
{
	const int  i = 10;
	int *ip = (int *)&i;
	
	printf("Previous: %p -> %d\n", &i, i);
	*ip = 20;
	printf("After: %p - > %d\n", ip, i);
	return 0;
}
