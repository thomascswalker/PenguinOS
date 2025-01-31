#include <bitarray.h>
#include <math.h>
#include <memory.h>
#include <stdio.h>

typedef BitArray<uint32_t> BlockMap;

static BlockMap*	  bitmap;
static uint32_t		  totalBlocks;
static Memory::Block* head;
static void*		  memorySpace;

void* kmalloc(uint32_t size)
{
	if (size == 0)
	{
		return nullptr;
	}
	return Memory::allocate(size);
}

// Increments the size of the program break by increment.
void* ksbrk(uint32_t increment)
{
	return nullptr;
}

// Sets the program break to the location specified by end_data_segment.
uint32_t kbrk(void* end_data_segment)
{
	return 0;
}

void Memory::init(uint32_t address, uint32_t size)
{
	debug("Initializing Physical Memory: %x => %x (%dMB)", address, address + size,
		(size / 1024 / 1024));

	bitmap = (BlockMap*)((uint32_t*)address);
	*bitmap = BlockMap(512); // TODO: Figure out a better sizing methodology

	uint32_t blockCount = bitmap->bitSize();
	uint32_t blockMapByteSize = blockCount * BLOCK_SIZE;

	memorySpace = (uint32_t*)(address + 0x100000);
}

void* Memory::allocate(uint32_t size)
{
	uint32_t requested = ALIGN(size, 4096) / 4096;
	int32_t	 block = bitmap->firstContiguous(requested);
	for (uint32_t i = 0; i < requested; i++)
	{
		bitmap->set(block + i);
	}

	// Return a void pointer to the Block we just constructed.
	uint32_t offset = block * BLOCK_SIZE;
	return (void*)((uint8_t*)memorySpace + offset);
}
