#include <pmm.h>

static BlockAllocator* allocator;

static uint8_t* heapStart;
static uint8_t* heapEnd;
static size_t	heapSize;

static uint32_t mallocAddress;

BlockAllocator::BlockAllocator(uint32_t inBlockCount)
{
	maxBlocks = inBlockCount;
	bitmap = BitArray<uint32_t>(inBlockCount);
	bitmap.set(0);
}

int32_t BlockAllocator::getNext(const uint32_t blockCount)
{
	uint32_t* memoryMap = allocator->bitmap.data();

	// Test 32 blocks at a time
	for (uint32_t chunk = 0; chunk < maxBlocks / BLOCKS_PER_BYTE; chunk++)
	{
		if (memoryMap[chunk] == 0xFFFFFFFF)
		{
			continue;
		}

		// Otherwise test each bit.
		for (uint32_t shift = 0; shift < BLOCKS_PER_BYTE; shift++)
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
				if ((shift + count > (BLOCKS_PER_BYTE - 1))
					&& (chunk + 1 <= maxBlocks / BLOCKS_PER_BYTE))
				{
					// If the current bit is free, increment.
					if (!(memoryMap[chunk + 1] & (1 << ((shift + count) - BLOCKS_PER_BYTE))))
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
					return (chunk * BLOCKS_PER_BYTE) + shift;
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
	uint32_t address = (startBlock * BLOCK_SIZE) + VIRTUAL_START + 0x100000; // TODO: Why +0x100000?
	return (uint32_t*)address;
}

void BlockAllocator::free(uint32_t* address, uint32_t blockCount)
{
	// debug("Freeing %d blocks at %x.", blockCount, address);
	uint32_t start = (uint32_t)address / BLOCK_SIZE;

	for (uint32_t i = 0; i < blockCount; i++)
	{
		bitmap.reset(start + i);
	}

	usedBlocks -= blockCount;
}

void BlockAllocator::initRegion(uint32_t address, uint32_t size)
{
	// debug("Initializing region at %x for %d bytes.", address, size);
	uint32_t align = address / BLOCK_SIZE;
	uint32_t blockCount = size / BLOCK_SIZE;

	while (blockCount > 0)
	{
		bitmap.reset(align++);
		usedBlocks--;
		blockCount--;
	}

	bitmap.set(0);
}

void BlockAllocator::deinitRegion(uint32_t address, uint32_t size)
{
	// debug("Deinitializing region at %x for %d bytes.", address, size);
	uint32_t align = address / BLOCK_SIZE;
	uint32_t blockCount = size / BLOCK_SIZE;

	while (blockCount > 0)
	{
		bitmap.set(align++);
		usedBlocks++;
		blockCount--;
	}
}

void PMM::init(uint32_t address, uint32_t memorySize)
{
	// Initialize the heap at the end of the kernel.
	heapStart = (uint8_t*)ALIGN(address, BLOCK_SIZE);

	// Compute the maximum number of blocks which can fit
	// in the requested memory space.
	uint32_t maxBlocks = ceildiv(memorySize, BLOCK_SIZE);

	// Given the maximum number of blocks we can allocate, compute
	// the heap end.
	heapSize = maxBlocks * BLOCK_SIZE;
	heapEnd = heapStart + heapSize;

	info("Heap is from %x to %x (%dMB).", heapStart, heapEnd, heapSize / MB);

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
	// Move the allocator pointer to the heap start
	allocator = (BlockAllocator*)heapStart + 0x10000;
	// Construct a new BlockAllocator at the allocator pointer
	*allocator = BlockAllocator(maxBlocks);
}

BlockAllocator* PMM::getAllocator()
{
	return allocator;
}

void* PMM::kmalloc(uint32_t size)
{
	uint32_t blockCount = ceildiv(size, BLOCK_SIZE);
	auto	 allocator = PMM::getAllocator();
	uint32_t address = (uint32_t)allocator->allocate(blockCount);
	void*	 tmp = (void*)address;
	memset(tmp, 0, size);
	return tmp;
}
