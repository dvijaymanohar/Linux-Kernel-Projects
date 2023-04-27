/* This file contains the functions mymalloc() and myfree() which imitate the standard malloc() and free() functions */

#include <stdio.h>
#include <stdlib.h>
#include "mem_man.h"

static struct memory_information meminfo;
static const int mask[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};


void *mymalloc(void)
{
	int i, j;

	if(meminfo.mem_remain == 0)
	{
		printf("Error: Memory full\n");
		return NULL;
	}

	if(meminfo.mem_remain < (meminfo.max_mem / 20))  //when only 5% of memory remaining
		printf("Warning: Low disk space\n");

	//search for the free memory space
	for(i = 0; i < meminfo.max_mem; i++)
		for(j = 0; j < 8; j++)
			if(!(*((char *)meminfo.mem_status_start + i) & mask[j]))
			{
				*((char *)meminfo.mem_status_start + i) |= mask[j];
				meminfo.mem_remain--;
				*((int *)meminfo.alloc_mem_start + (i * 8) + j) = 0x00;	//init
				return((int *)meminfo.alloc_mem_start + (i * 8) + j);
			}
	return NULL;
}

                
void myfree(void *free_pos)
{
	int index = free_pos - meminfo.alloc_mem_start;
	printf("index = %d\n", index);
	
	if(index < 0 || ((index / 4) > (meminfo.max_mem - 1)))
	{
		printf("Error: Segmemtation fault\n");
		exit(1);
	}

	if(index == 0)
	{
		printf("Error: Memory in use, cannot free it now\n");
	}
	else if(*((char *)meminfo.mem_status_start + (index / 8)) & mask[index % 8])
	{
		*((char *)meminfo.mem_status_start + (index / 8)) &= (~mask[index % 8]);		
		free(free_pos);
		meminfo.mem_remain++;
	}
	else
		printf("Error: Cannot free the memory that is not allocated to you\n");
}


void init_mem_man(void)
{
	printf("\nEnter the memory required(positive interger) in terms of pointer size: ");
	scanf("%u", &(meminfo.max_mem));
	printf("max_mem = %u\n", meminfo.max_mem);
	while(getchar() != '\n');

	meminfo.mem_remain = meminfo.max_mem;
	printf("mem_remain = %u\n", meminfo.mem_remain);

	meminfo.alloc_mem_start = calloc(meminfo.max_mem , sizeof(void *));

	meminfo.mem_status_start = calloc(((meminfo.max_mem / 8) + 1) , sizeof(char));
}


void print(void)
{
	char temp;
	int i;

	printf("\nalloc_address\tstatus_address\tstatus\n");
	for(i = 0; i < meminfo.max_mem; i++)
	{
		temp = !!(*((char *)meminfo.mem_status_start + (i / 8)) & mask[i]);
		printf("%p\t%p\t%d\n", ((int *)meminfo.alloc_mem_start + i), ((char *)meminfo.mem_status_start + ( i / 8)), temp);
	}
}
