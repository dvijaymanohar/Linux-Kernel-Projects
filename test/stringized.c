#include <stdio.h>

#define print(expr) printf(#expr "= %s", "yes");
#define CAT(x, y) (x##y)

int main()
{
	int CAT(x, 0);

	CAT(x, 0) = 5;
	print(CAT("Hello", "World"));
	printf("\n%d\n", CAT(x, 0));
	return 0;
}
