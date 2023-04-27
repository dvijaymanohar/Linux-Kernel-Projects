/* Header file for the memory manager program */

#ifndef MEM_MAN_H
#define MEM_MAN_H

	struct memory_information
	{
		unsigned int max_mem, mem_remain;
		void *alloc_mem_start;
		void *mem_status_start;
	};

	void *mymalloc(void);
	void myfree(void *);
	void init_mem_man(void);
	void print(void);

#endif
