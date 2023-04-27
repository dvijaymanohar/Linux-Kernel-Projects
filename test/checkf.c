#include <stdio.h>

typedef struct Node tNode;
struct Node
{
	int val;
}
tNode abc;

int main()
{
	abc.val = 10;
	printf("%d", abc.val);
	return 0;
}
