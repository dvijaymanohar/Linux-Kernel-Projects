#include <stdio.h>

int main()
{
	int i = 5;
	
	printf("%d%d", sizeof(i++), i);
	return 0;
}
