/* Trail programs to clear the concept */

#include <stdio.h>


//integer assignment to pointer
#define MINOR(i) i

int main()
{
	unsigned int i = 1;

	//void *private_data;
	//private_data = ONE;
	
	struct fil{
		void *private_data;
	};
	struct fil *filp;

	filp->private_data = (void *)MINOR(i);
	printf("%u\t%u\n", (unsigned int)filp->private_data, i);

	return 0;
}
