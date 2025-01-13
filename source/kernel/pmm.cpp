#include <pmm.h>

static bool		initialized = false;
static uint8_t* bitmap = (uint8_t*)&kernelEnd;
static uint8_t* memoryStart = nullptr;
static uint32_t totalBlocks = 0;
static uint32_t bitmapSize = 0;

void PMM::init(uint32_t memorySize)
{
	debug("Bitmap addr: %x", (uint32_t)bitmap);
	totalBlocks = ceildiv(memorySize, BLOCK_SIZE);
	bitmapSize = ceildiv(totalBlocks, BLOCKS_PER_BUCKET);

	debug("Total blocks: %d", totalBlocks);
	debug("Bitmap size: %d", bitmapSize);
	memset(bitmap, 0, bitmapSize);

	memoryStart = (uint8_t*)BLOCK_ALIGN(((uint32_t)bitmap + bitmapSize));
	debug("Memory start: %x", memoryStart);
	initialized = true;
}

uint32_t PMM::getFirstFreeBlock()
{
	for (uint32_t i = 0; i < totalBlocks; i++)
	{
		if (!IS_SET(i))
		{
			return i;
		}
	}
	panic("Ran out of free blocks.");
	return -1;
}

uint32_t PMM::allocateBlock()
{
	uint32_t block = getFirstFreeBlock();
	SET_BIT(block);
	return block;
}

void PMM::freeBlock(uint32_t block)
{
	CLEAR_BIT(block);
}

uint32_t PMM::getFreeBlockCount()
{
	uint32_t count = getTotalBlockCount();
	for (uint32_t i = 0; i < getTotalBlockCount(); i++)
	{
		if (IS_SET(i))
		{
			count--;
		}
	}
	return count;
}

bool PMM::isInitialized()
{
	return initialized;
}

void* PMM::getMemoryStart()
{
	return memoryStart;
}

uint32_t PMM::getTotalBlockCount()
{
	return totalBlocks;
}

uint32_t PMM::getBitmapSize()
{
	return bitmapSize;
}

uint8_t* PMM::getBitmap()
{
	return bitmap;
}
