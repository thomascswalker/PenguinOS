#include <memory.h>

static uint32_t nextFreeIndex = 0;

void* kmalloc(size_t size)
{
	void* ptr = (void*)((uint8_t*)KMALLOC_MEMORY_ADDRESS + (nextFreeIndex * PAGE_SIZE));
	nextFreeIndex++;
	return ptr;
}
