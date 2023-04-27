#include <stdio.h>

int main()
{
	int i;
	struct ex1
	{
		char a[2];
		int x;
		char b[2];
	};
	i = sizeof(struct ex1);
	printf("P -> %d\n", i);	
	struct ex2
	{
		char a[2];
		char b[2];
		int x;
	};	
	i = sizeof(struct ex2);
	printf("L -> %d\n", i);	
	return 0;
}
