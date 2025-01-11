#pragma once

#include <ctype.h>

// Single linked list
typedef struct malloc_block
{
	uint32_t			 size; // Size of this memory block in bytes
	bool				 free; // Is this block free or not?
	struct malloc_block* next; // The next block

} malloc_block_t;

static malloc_block_t* malloc_list_head = 0;
static uint32_t		   malloc_vaddr = 0x100000;
static uint32_t		   malloc_paddr = 0;
static uint32_t		   malloc_page_count = 0;

void  init_malloc(size_t bytes);
void  malloc_split(malloc_block_t* block, uint32_t size);
void* malloc_next_block(size_t bytes);
void  merge_free_blocks();
void  malloc_free(void* ptr);
bool  validate_malloc();