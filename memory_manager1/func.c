/* This file contains the functions mymalloc() and myfree() which imitate the standard malloc() and free() functions */

#include <stdio.h>
#include <stdlib.h>

//struct memory_information *meminfo;
static const int mask[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
static unsigned int max_mem, mem_remain;
static void *alloc_mem_start, *mem_status_start;

void *mymalloc(void)
{
	int i, j;

	if(mem_remain == 0)
	{
		printf("Error: Memory full\n");
		return NULL;
	}

	if(mem_remain < (max_mem / 20))  //when only 5% of memory remaining
		printf("Warning: Low disk space\n");

	//search for the free memory space
	for(i = 0; i < max_mem; i++)
		for(j = 0; j < 8; j++)
			if(*(char *)(mem_status_start + i) & mask[j])
			{
				*(char *)(mem_status_start + i) |= mask[j];
				mem_remain--;
				*(int *)(alloc_mem_start + (i * 8) + j) = 0x00;	//init
				return((void *)(alloc_mem_start + (i * 8) + j));
			}
	return NULL;
}


void myfree(void *free_pos)
{
	int index = free_pos - alloc_mem_start;
	
	if(index < 0 || index > max_mem)
	{
		printf("Error: Segmemtation fault\n");
		exit(1);
	}
	
	if(*(char *)(mem_status_start + (index / 8)) & mask[index % 8])
	{
		*(char *)(mem_status_start + (index / 8)) &= (~mask[index % 8]);		
		free(free_pos);
		mem_remain++;
		return;
	}
}


void init_mem_man(void)
{
	printf("\nEnter the memory required(positive interger) in terms size of pointer: ");
	scanf("%u", &(max_mem));
	while(getchar() != '\n');

	mem_remain = max_mem;

	alloc_mem_start = malloc(max_mem * sizeof(void *));

	mem_status_start = malloc(((max_mem / 8) + 1) * sizeof(char));
}


void print(void)
{
	
