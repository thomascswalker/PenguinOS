#include <pmm.h>

static BlockAllocator allocator(0);
static uint8_t*		  heapStart;
static uint8_t*		  heapEnd;
static size_t		  heapSize;

BlockAllocator::BlockAllocator(uint32_t inBlockCount)
{
	maxBlocks = inBlockCount;
	bitmap = BitArray<uint32_t>(inBlockCount);
	// bitmap.fill(true);
}

int32_t BlockAllocator::getNext(const uint32_t blockCount)
{
	uint32_t* memoryMap = allocator.bitmap.data();

	// Test 32 blocks at a time
	for (uint32_t chunk = 0; chunk < maxBlocks / 32; chunk++)
	{
		if (memoryMap[chunk] == 0xFFFFFFFF)
		{
			continue;
		}

		// Otherwise test each bit.
		for (uint32_t shift = 0; shift < 32; shift++)
		{
			// The bit we are currently testing.
			uint32_t bit = 1 << shift;

			// If this bit is set, continue.
			if ((memoryMap[chunk] & bit))
			{
				continue;
			}

			// Otherwise this bit is free and we want to
			// keep going until we find the next set bit or
			// we hit the requested block count.
			for (uint32_t count = 0, freeBlocks = 0; count < blockCount; count++)
			{
				// Since we are going from n..31, check if we are still within range.
				// As well, check to make sure we haven't exceeded our max block count.
				if ((shift + count > 31) && (chunk + 1 <= maxBlocks / 32))
				{
					// If the current bit is free, increment.
					if (!(memoryMap[chunk + 1] & (1 << ((shift + count) - 32))))
					{
						freeBlocks++;
					}
				}
				else
				{
					// If the current bit is free, increment.
					if (!(memoryMap[chunk] & (1 << (shift + count))))
					{
						freeBlocks++;
					}
				}

				// If we've hit the target block count, we've
				// found a contiguous set of blocks which can
				// accomodate our requested block count.
				if (freeBlocks == blockCount)
				{
					// Return the starting block index.
					return (chunk * 32) + shift;
				}
			}
		}
	}

	// Out of memory
	return -1;
}

uint32_t* BlockAllocator::allocate(uint32_t blockCount)
{
	// If there are no more available blocks, return
	if ((maxBlocks - usedBlocks) <= blockCount)
	{
		error("No more blocks available.");
		return nullptr;
	}

	uint32_t startBlock = getNext(blockCount);
	if (startBlock == -1)
	{
		error("Unable to find contiguous block sequence of size %d.", blockCount);
		return nullptr;
	}

	// Set all of the blocks within this sequence to true
	for (uint32_t i = startBlock; i < startBlock + blockCount; i++)
	{
		bitmap.set(i);
	}

	// Increment the used block count by the requested block count
	usedBlocks += blockCount;

	// Return the address of the starting block in this sequence
	uint32_t address = startBlock * BLOCK_SIZE;
	return (uint32_t*)address;
}

void BlockAllocator::free(uint32_t* address, uint32_t blockCount)
{
}

void BlockAllocator::initRegion(uint32_t address, uint32_t size)
{
}

void BlockAllocator::deinitRegion(uint32_t address, uint32_t size)
{
}

void PMM::init(uint32_t address, uint32_t memorySize)
{
	// Initialize the heap at the end of the kernel.
	heapStart = (uint8_t*)ALIGN(address, BLOCK_SIZE);
	debugx(heapStart);

	// Compute the maximum number of blocks which can fit
	// in the requested memory space.
	uint32_t maxBlocks = ceildiv(memorySize, BLOCK_SIZE);
	debugd(maxBlocks);

	// Given the maximum number of blocks we can allocate, compute
	// the heap end.
	heapSize = maxBlocks * BLOCK_SIZE;
	debugd(heapSize / MB);
	heapEnd = heapStart + heapSize;
	debugx(heapEnd);

	/*
	Create a new block allocator.

	Each bit in the bitmap maps to a block on the heap.
	A value of 0 means the block is free, and a value of 1
	means the block is used.
	---------------------------------------
	|   0   |   0   |   0   |   0   | ... |
	---------------------------------------
		v       v       v       v
	---------------------------------------
	| 4096b | 4096b | 4096b | 4096b | ... |
	---------------------------------------
	*/

	// Initialize the bitmap
	allocator = BlockAllocator(maxBlocks);
}

BlockAllocator* PMM::getAllocator()
{
	return &allocator;
}

// Allocates physical memory.
void* PMM::kmalloc(uint32_t size)
{
	uint32_t blockCount = ceildiv(size, BLOCK_SIZE);
	return allocator.allocate(blockCount);
}

// Frees physical memory.
void PMM::kfree(void* ptr)
{
}

void PMM::kdebug()
{
}
