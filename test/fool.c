#include <stdio.h>

void foo(char * y)
{
	y = malloc(20);
}

int main()
{
	int *x;
	int i;

	foo(x);	
	for(i = 0; i < 20; i++)
	{
		*(x + i) = i;
		printf("%d\n", *(x + i));
	}
	return 0;
}
