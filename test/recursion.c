#include <stdio.h>

int func(int a)
{
	if(a <= 1)
		return 1;
	else
		return (func(a -3) + func(a -1));
}
int main()
{
	int n = 6, ret;

	ret = func(n);
	printf("%d\n", ret);
	return 0;
}
