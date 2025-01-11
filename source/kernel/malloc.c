#include <malloc.h>
#include <memory.h>

void init_malloc(const size_t bytes)
{
	info("Initializing malloc with %d bytes.", bytes);

	// Determine the number of pages we need to allocate
	// given the number of bytes requested.
	malloc_page_count = ceildiv(bytes, PAGE_SIZE);
	if (bytes % PAGE_SIZE > 0)
	{
		malloc_page_count++; // Partial page
	}

	// Allocate the blocks for this request
	debug("Allocating %d blocks.", malloc_page_count);
	malloc_paddr = (uint32_t)allocate_blocks(malloc_page_count);
	debug("Allocated physical address at %x.", (uint32_t*)malloc_paddr);

	// The list head will start at the beginning of the block itself.
	// 0         11|12        ?|
	// -------------------------
	// | list_head | block ... |
	// -------------------------
	malloc_list_head = (malloc_block_t*)malloc_vaddr;
	if (!validate_malloc())
	{
		panic("Unable to initialize malloc.");
	}

	// Map in pages
	uint32_t vaddr = malloc_vaddr;
	debug("Malloc page count: %d", malloc_page_count);
	for (uint32_t i = 0; i < malloc_page_count; i++)
	{
		paddr_t paddr = malloc_paddr + (i * PAGE_SIZE);
		if (!map_page((void*)paddr, (void*)vaddr))
		{
			panic("Unable to map page %d.", i);
		}

		pt_entry_t* page = get_page(vaddr);
		SET_ATTRIBUTE(page, PTE_READ_WRITE); // Read/write access for malloced pages.

		vaddr += PAGE_SIZE;
	}

	if (malloc_list_head)
	{
		// Each page starts with the malloc linked list, so we get the total size and subtract
		// the size of the list itself.
		malloc_list_head->size = (malloc_page_count * PAGE_SIZE) - sizeof(malloc_block_t);
		malloc_list_head->free = true;
		malloc_list_head->next = nullptr;

		success("Malloc initialized: addr=%x, size=%d", (uintptr_t)malloc_list_head,
			malloc_list_head->size);
		return;
	}

	panic("Unable to intialize memory allocation.");
}

void malloc_split(malloc_block_t* block, const uint32_t size)
{
	debug("Splitting block at %x to match requested size %d.", (uint32_t*)block, size);
	malloc_block_t* new_block = (malloc_block_t*)((void*)block + size + sizeof(malloc_block_t));
	debug("Sequential block is at %x.", (uint32_t*)new_block);

	// New node size is the difference of the original node size minus
	// the requested size minus the size of the linked list node.
	new_block->size = block->size - size - sizeof(malloc_block_t);
	new_block->free = true;
	new_block->next = block->next;
	debug("New (second) block: size=%d, free=%d, next=%x", //
		new_block->size, new_block->free, new_block->next);

	block->size = size;
	block->free = false;
	block->next = new_block;
	debug("Target (first) block: size=%d, free=%d, next=%x", //
		block->size, block->free, block->next);
}

void* malloc_next_block(const size_t bytes)
{
	if (bytes == 0)
	{
		return nullptr;
	}

	if (!malloc_list_head->size)
	{
		init_malloc(bytes);
	}

	// Find first available block to allocate
	debug("Finding next block for %d bytes.", bytes);
	malloc_block_t* current = malloc_list_head;
	debug("Starting block search at %x.", (uint32_t*)current);
	if (!current)
	{
		panic("Memory list error.");
	}

	// Find a valid empty and unused block which is smaller
	// than the requested block size.
	// While the current block size is smaller than the requested
	// byte count or is not free, and the next block is valid,
	// set the current to the next.
	while (((current->size < bytes) || !current->free) && current->next)
	{
		current = current->next;
	}
	debug("Next block is at %x.", (uint32_t*)current);

	// Node size matches requested size
	if (bytes == current->size)
	{
		debug("Block size is equal to requested byte size (%d).", bytes);
		current->free = false;
	}
	// Node size is larger than requested size
	else if (current->size > bytes + sizeof(malloc_block_t))
	{
		debug("Block size (%d) is larger than requested block size (%d).", current->size, bytes);
		malloc_split(current, bytes);
	}
	// Node size is smaller than requested size
	else
	{
		debug("Block size (%d) is smaller than requested block size (%d).", current->size, bytes);
		uint32_t page_count = 1;

		// Keep incrementing page count until we exceed the requested
		// byte size.
		while (current->size + (page_count * PAGE_SIZE) < bytes + sizeof(malloc_block_t))
		{
			page_count++;
		}

		// Allocate & map in new pages starting at the next vaddr page boundary
		uint32_t virt = malloc_vaddr + (malloc_page_count * PAGE_SIZE);
		debug("Allocating and mapping %d pages.", page_count);
		for (uint8_t i = 0; i < page_count; i++)
		{
			// Allocate a new 4KB block of RAM and return the address to that.
			pt_entry_t page;
			uint32_t*  temp = allocate_page(&page);
			map_page(temp, (void*)virt);
			SET_ATTRIBUTE(&page, PTE_READ_WRITE);
			virt += PAGE_SIZE;
			current->size += PAGE_SIZE;
			malloc_page_count++;
		}

		debug("Splitting current.");
		malloc_split(current, bytes);
	}

	// Return node pointing to requested bytes, casted to void* to remove pointer arithmetic
	return (void*)current + sizeof(malloc_block_t);
}

void merge_free_blocks()
{
	debug("Merging free blocks.");
	malloc_block_t* current = malloc_list_head;
	uint32_t		merged = 0;

	while (current && current->next)
	{
		// Two consecutive nodes that are free
		if (current->free && current->next->free)
		{
			current->size += (current->next->size + sizeof(malloc_block_t));
			current->next = current->next->next; // Remove 'next' node from list
			merged++;
		}
		current = current->next;
	}
	debug("Merged %d blocks.", merged);
}

void malloc_free(void* ptr)
{
	if (!ptr)
	{
		warning("Pointer at %x is NULL.", (uint32_t*)ptr);
		return;
	}

	malloc_block_t* current = malloc_list_head;
	while (current->next)
	{
		// Found block we are freeing
		if ((void*)current + sizeof(malloc_block_t) == ptr)
		{
			debug("Found block to free at %x.", (uint32_t*)current);
			current->free = true;
			merge_free_blocks();
			return;
		}
		current = current->next;
	}
}

bool validate_malloc()
{
	return (malloc_list_head != 0);
}
