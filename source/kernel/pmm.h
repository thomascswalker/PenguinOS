#pragma once

#include <math.h>
#include <stdio.h>

#define BLOCK_SIZE 4096
#define BLOCKS_PER_BUCKET 8

#define BLOCK_ALIGN(addr) (((addr) & 0xFFFFF000) + 0x1000)
#define SET_BIT(i) \
	bitmap[i / BLOCKS_PER_BUCKET] = bitmap[i / BLOCKS_PER_BUCKET] | (1 << (i % BLOCKS_PER_BUCKET))
#define CLEAR_BIT(i)                \
	bitmap[i / BLOCKS_PER_BUCKET] = \
		bitmap[i / BLOCKS_PER_BUCKET] & (~(1 << (i % BLOCKS_PER_BUCKET)))
#define IS_SET(i) ((bitmap[i / BLOCKS_PER_BUCKET] >> (i % BLOCKS_PER_BUCKET)) & 0x1)
#define GET_BUCKET32(i) (*((uint32_t*)&bitmap[i / 32]))

EXTERN uint32_t kernelStart; // Defined in linker.ld
EXTERN uint32_t kernelEnd;	 // Defined in linker.ld

namespace PMM
{

	void	 init(uint32_t memorySize);
	uint32_t getFirstFreeBlock();
	uint32_t allocateBlock();
	void	 freeBlock(uint32_t block);
	uint32_t getFreeBlockCount();

	bool	 isInitialized();
	void*	 getMemoryStart();
	uint32_t getTotalBlockCount();
	uint32_t getBitmapSize();
	uint8_t* getBitmap();
}; // namespace PMM