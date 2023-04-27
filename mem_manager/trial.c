/* This file contains programs that were used to clear the concepts */

#include <stdio.h>
#include <stdlib.h>


//To clear the concept of memory allocation for bits used in structures
/*
struct onebit
{
	unsigned int flag: 1;
	unsigned int flag1: 1;
	unsigned int flag2: 1;
	unsigned int flag3: 1;
	unsigned int flag4: 1;
	unsigned int flag5: 1;
	unsigned int flag6: 1;
	unsigned int flag7: 1;
	unsigned int flag8: 1;
	unsigned int flag9: 1;
	unsigned int flag0: 1;
};

int main()
{
	struct onebit a;
	int i;
	
/	for(i = 0; i < 64; i += 2)
		a[i].flag = 0;

	for(i = 1; i < 64; i += 2)
		a[i].flag = 1;

	for(i = 0; i <64; i++)
		printf("\na[%d] = %d is located at %p", i, a[i].flag, &a[i].flag);
/
	printf("sizeof(struct onebit) = %d\n", sizeof(a));

	return 0;
}
*/


/*
//Pointer arithmatic

int main()
{
	void *mem, *mem1;
	void *a5;

	mem = malloc(sizeof(void *));
	mem1 = malloc(sizeof(void *));
	a5 = malloc(5 * sizeof(void *));

	printf("mem points to %p\n", mem);
	printf("mem1 points to %p\n", mem1);
	printf("a5 points to %p\n", a5);

	printf("mem1 - mem = %d\n", mem1 - mem);
	printf("mem - mem1 = %d\n", mem - mem1);
	printf("a5 + 0 = %p\n", (a5 + 0));
	printf("a5 + 1 = %p\n", (a5 + 1));
	printf("a5 + 4 = %p\n", (a5 + 4));
	printf("a5 + 5 = %p\n", (a5 + 5));

	mem = a5;
	mem1 = a5 + 4;
	printf("a5 - (a5 + 4) = %d\n", mem - mem1);

	return 0;
}
*/

/*
//freeing unallocated memory

int main()
{
	int *ptr;

	free(ptr);

	return 0;
}
*/


//compliment of a number

int main()
{
	char i = 0xF0;

	printf("~0xF0 = %X\n !0xF0 = %X", ~i, !i);

	return 0;
}
