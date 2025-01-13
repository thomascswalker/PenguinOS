#include <heap.h>
#include <stdio.h>

struct FreeChunk
{
	size_t	   size;
	FreeChunk* next;
	FreeChunk* prev;
};

void* Heap::kmalloc(uint32_t size)
{
	if (!size)
	{
		return nullptr;
	}

	if (!PMM::isInitialized())
	{
		error("PMM not initialized.");
		return nullptr;
	}

	uint32_t blockCount = ceildiv(size, BLOCK_SIZE);
	debugi(blockCount);

	uint32_t first = PMM::getFirstFreeBlock();
	for (uint32_t i = 0; i < blockCount; i++)
	{
		PMM::allocateBlock();
	}

	return PMM::getBitmap() + first;
}

void Heap::kfree(void* ptr)
{
	auto bitmap = PMM::getBitmap();
	for (uint32_t i = 0; i < PMM::getTotalBlockCount(); i++)
	{
		uint32_t* block = (uint32_t*)(bitmap + i);
		if (block == ptr)
		{
			while (IS_SET(i))
			{
				CLEAR_BIT(i);
				i++;
			}
		}
	}
}
