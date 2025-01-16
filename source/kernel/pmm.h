#pragma once

#include <bitarray.h>
#include <math.h>
#include <stdio.h>

#define VIRTUAL_START 0xC0000000 // 3GB + 1MB
#define BLOCK_SIZE 0x1000
#define BLOCKS_PER_BYTE 32
#define ALIGN(addr, size) (((uintptr_t)addr + size - 1) & ~(size - 1))

EXTERN uint32_t kernelStart; // Defined in linker.ld
EXTERN uint32_t kernelEnd;	 // Defined in linker.ld

struct Block
{
	size_t	 size;
	uint32_t index;
	bool	 free;
	Block*	 next;

	static Block* create(void* address, uint32_t size, uint32_t index)
	{
		debug("Creating block at %x.", address);
		auto b = (Block*)(address);
		b->size = size;
		b->index = index;
		b->free = true;
		b->next = nullptr;
		return b;
	}

	size_t start()
	{
		return (size_t)this;
	}

	size_t end()
	{
		return (size_t)this + size;
	}
};

struct BlockAllocator
{

	Block*			   blocks = nullptr;
	uint32_t		   usedBlocks = 0;
	uint32_t		   maxBlocks = 0;
	BitArray<uint32_t> bitmap;

	BlockAllocator(uint32_t inByteSize);
	int32_t	  getNext(const uint32_t count);
	uint32_t* allocate(uint32_t blockCount);
	void	  free(uint32_t* address, uint32_t blockCount);
	void	  initRegion(uint32_t address, uint32_t size);
	void	  deinitRegion(uint32_t address, uint32_t size);
};

namespace PMM
{
	void			init(uint32_t address, uint32_t memorySize);
	BlockAllocator* getAllocator();

	void* kmalloc(uint32_t size);
	void  kfree(void* ptr);
	void  kdebug();
}; // namespace PMM