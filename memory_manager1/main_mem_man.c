/* This program is used to verify mymalloc() and myfree() functions */

#include <stdio.h>
#include "mem_man.h"

int main()
{
	unsigned int choise, word;
	void *ptr;

	init_mem_man();		//initialize the memory information
	
	while(1)
	{
		choise = 100;	//any value other than required
		printf("\n1 : mymalloc()\
			\n2 : myfree()\
			\n3 : Exit the program\n");

		printf("\nEnter your choise : ");
		scanf("%u", &choise);
		while(getchar() != '\n');		//clear the input buffer

		switch(choise)
		{
			case 1 : ptr = mymalloc();
				 break; 

			case 2 : printf("Enter the word no. to be freed : ");
				 scanf("%u", &word);
				 myfree(ptr + word);
				 break; 

			case 3 : return 0;

			default : printf("Enter a valid a valid digit\n");
		}
	}
}
