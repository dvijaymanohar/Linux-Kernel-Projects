#include <stdio.h>
#include <time.h>

int main()
{
	int  *ip = (int *)TIMER_ADDRESS;
	int time, i;

	for(i = 0; i < 10; i++)
	{
		time = *ip;
		printf("The current time is = %d\n", time);
	}
	return 0;
}
