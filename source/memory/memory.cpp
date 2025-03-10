/*

Paging directory memory is located at:
[PD .. PD + 4096 + (1024 * 4096)] (+4096 is the size of the PDEs)
[0x110000 .. 0x511000] == 4MB + 4KB of memory space

Paging Memory
---------------------------------
| PD | PT0 | PT1 | ... | PT1023 |
---------------------------------

Page Directory (first 4K bytes)
--------------------------------------
| PDE0 | PDE1 | PDE2 | ... | PDE1023 |
--------------------------------------

Page Directory Entry (4 bytes)
See the PDE enum for details.
--------------------------------------------------
| PT Address | PS | D | A | NC | W | US | RW | P |
--------------------------------------------------
31          8| 7  | 6 | 5 |  4 | 3 |  2 |  1 | 0 |


Page Table (PD + (N * 4K), 4K bytes long)
--------------------------------------
| PTE0 | PTE1 | PTE2 | ... | PTE1023 |
--------------------------------------

Page Table Entry (4 bytes)
See the PTE enum for details.
-----------------
| P Address | A |
-----------------
31         2|1  0

*/

#include <math.h>
#include <memory.h>
#include <stdio.h>

// Defined in linker.ld
EXTERN uint32_t kernelStart;
EXTERN uint32_t kernelEnd;

static uint32_t memoryStart;
static uint32_t memoryEnd;
static uint32_t memorySize;
static void*	memoryPool;

// An array of bucket sizes corresponding to orders 0..NUM_BUCKETS-1.
// Order 0: 32 bytes, order 1: 64 bytes, ..., order 7: 4096 bytes.
static const size_t bucketSizes[BUCKET_COUNT] = {
	1 << MIN_ORDER,		  // 32 bytes
	1 << (MIN_ORDER + 1), // 64 bytes
	1 << (MIN_ORDER + 2), // 128 bytes
	1 << (MIN_ORDER + 3), // 256 bytes
	1 << (MIN_ORDER + 4), // 512 bytes
	1 << (MIN_ORDER + 5), // 1024 bytes
	1 << (MIN_ORDER + 6), // 2048 bytes
	1 << (MIN_ORDER + 7)  // 4096 bytes
};

static Block* freeList[BUCKET_COUNT];

// The page directory is constructed at an arbitrary location, but
// most notably it is REQUIRED to be page-aligned (aligned to 4096).
// 0x110000 is the (likely) location in memory.
static uint32_t pageDirectory[1024] __attribute__((aligned(PAGE_SIZE)));

void Memory::init(uint32_t start, uint32_t size)
{
	// Zero out all entries in the page directory.
	memset(pageDirectory, 0, 1024 * sizeof(uint32_t));

	for (uint32_t i = 0; i < TABLE_COUNT; i++)
	{
		// Compute the address of the page table. Table 0 starts
		// immediately after the page directory ends [PD + 0x1000].
		// Each table is 0x1000 bytes, so each table is offset
		// [i * 0x1000] from table 0.
		uint32_t  address = PAGE_ALIGN((uint32_t)pageDirectory + PAGE_SIZE + (i * PAGE_SIZE));
		uint32_t  index = PD_INDEX(address);
		uint32_t* entry = &pageDirectory[index];

		// Set each table to be both read/write.
		*entry = address | PDE_Write;
	}

	// Identity map the first 8MB of memory
	// 0x000000 => 0x3FFFFF
	// 0x400000 => 0x7FFFFF
	identityMapTable(0);
	identityMapTable(1);

	// Identity map the 768th table (at 0xC0000000)
	// 0xC0000000 => 0xC003FFFFF
	// 0xC0000000 >> 22 == 768
	identityMapTable(PD_INDEX(VIRTUAL_START));

	setPageDirectory(pageDirectory);

	// Disable 4MB paging.
	setLargePaging(false);

	// After everything is all setup, enable paging.
	// TODO: Figure out how to actually use paging correctly.
	// enablePaging();

	// Setup memory allocation
	// Page-aligned start and end of usable memory in the system.
	memoryStart = (uint32_t)pageDirectory + PAGE_SIZE + DIR_SIZE;
	memoryEnd = PAGE_ALIGN(memoryStart + (size - PAGE_SIZE - DIR_SIZE));
	memorySize = memoryEnd - memoryStart;
	memoryPool = (uint8_t*)memoryStart;

	// Buddy allocator
	size_t offset = 0;
	for (uint32_t order = BUCKET_COUNT - 1; order > 0; order--)
	{
		size_t blockSize = bucketSizes[order];
		while (offset + blockSize < memorySize)
		{
			Block* block = (Block*)((uint8_t*)memoryPool + offset);
			block->order = order;
			block->isFree = true;
			block->next = nullptr;

			addBlock(block);
			offset += blockSize;
		}
	}
}

void Memory::identityMapTable(uint32_t index)
{
	// Set this table to present.
	uint32_t* entry = &pageDirectory[index];
	*entry |= PDE_Present;

	// Because we're identity mapping (physical == virtual),
	// we can just unshift the table index to retrieve
	// the address this table is mapped to.
	uint32_t  offset = index << 22;
	uint32_t* table = getTableFromIndex(index);
	for (uint32_t i = 0; i < PAGE_COUNT; i++)
	{
		table[i] = offset + (i * PAGE_SIZE) | 0x1; // Active page
	}
}

uint32_t* Memory::getTableFromAddress(uint32_t address)
{
	return (uint32_t*)(pageDirectory[PD_INDEX(address)] & PAGE_MASK);
}

uint32_t* Memory::getTableFromIndex(uint32_t index)
{
	return (uint32_t*)(pageDirectory[index] & PAGE_MASK);
}

void Memory::enablePaging()
{
	uint32_t cr0;
	asm("mov %%cr0, %0" : "=r"(cr0));
	cr0 |= 0x80000001; // Paging + Protected Mode
	asm("mov %0, %%cr0" ::"r"(cr0));
}

void Memory::setPageDirectory(uint32_t* directory)
{
	asm("mov %0, %%cr3" ::"r"((uint32_t)directory));
}

void Memory::setLargePaging(bool state)
{
	uint32_t cr4;
	asm("mov %%cr4, %0" : "=r"(cr4));
	if (state)
	{
		cr4 |= 0x00000010;
	}
	else
	{
		cr4 &= ~0x00000010;
	}
	asm("mov %0, %%cr4" ::"r"(cr4));
}

void Memory::dumpPageTable()
{
	for (uint32_t i = 0; i < TABLE_COUNT; i++)
	{
		uint32_t* entry = &pageDirectory[i];
		if (!ENTRY_TEST(entry, PDE_Present))
		{
			continue;
		}
		uint32_t* table = getTableFromIndex(i);
		uint32_t  start = table[0] & PAGE_MASK;
		uint32_t  end = (table[1023] & PAGE_MASK) + 0x1000 - 1;
		printf("Table %d: %x => %x\n", i, start, end);
	}
}

void Memory::addBlock(Block* block)
{
	int order = block->order;
	block->next = freeList[order];
	freeList[order] = block;
}

void Memory::removeBlock(Block* block, uint32_t order)
{
	Block** current = &freeList[order];
	while (*current)
	{
		if (*current == block)
		{
			*current = block->next;
			block->next = nullptr;
			return;
		}
		current = &((*current)->next);
	}
}

void* Memory::kmalloc(size_t size)
{
	if (size == 0)
	{
		return nullptr;
	}

	uint32_t order;
	size_t	 totalSize = size + sizeof(Block);
	for (order = 0; order < BUCKET_COUNT; order++)
	{
		if (bucketSizes[order] >= totalSize)
		{
			break;
		}
	}

	if (order > MAX_ORDER)
	{
		warning("kmalloc: Order %d is greater than maximum order %d.", order, MAX_ORDER);
		return nullptr;
	}

	uint32_t currentOrder = order;
	while (currentOrder < BUCKET_COUNT && freeList[currentOrder] == nullptr)
	{
		currentOrder++;
	}

	if (currentOrder >= BUCKET_COUNT)
	{
		warning("kmalloc: Order >= %d.", BUCKET_COUNT);
		return nullptr;
	}

	Block* block = freeList[currentOrder];
	freeList[currentOrder] = block->next;
	block->next = nullptr;

	while (currentOrder > order)
	{
		currentOrder--;

		size_t newBlockSize = bucketSizes[currentOrder];

		Block* buddy = (Block*)((uint8_t*)block + newBlockSize);
		buddy->order = currentOrder;
		buddy->isFree = true;
		buddy->next = nullptr;

		Memory::addBlock(buddy);

		block->order = currentOrder;
	}

	block->isFree = false;

	return (void*)((uint8_t*)block + sizeof(Block));
}

void Memory::kfree(void* ptr)
{
	if (ptr == nullptr)
	{
		return;
	}

	Block* block = (Block*)((char*)ptr - sizeof(Block));
	block->isFree = true;

	uint32_t order = block->order;
	while (order < BUCKET_COUNT)
	{
		uintptr_t blockOffset = (uintptr_t)((char*)block - (char*)memoryPool);
		size_t	  blockSize = bucketSizes[order];

		uintptr_t buddyOffset = blockOffset ^ blockSize;
		if (buddyOffset + blockSize > memorySize)
		{
			break; // Buddy is out of bounds
		}

		Block* buddy = (Block*)((char*)memoryPool + buddyOffset);
		if (!buddy->isFree || buddy->order != order)
		{
			break; // Cannot merge
		}

		Memory::removeBlock(buddy, order);
		if (buddyOffset < blockOffset)
		{
			block = buddy;
			blockOffset = buddyOffset;
		}

		order++;
		block->order = order;
	}

	Memory::addBlock(block);
}

void* Memory::krealloc(void* ptr, size_t newSize)
{
	if (ptr == nullptr)
	{
		return kmalloc(newSize);
	}

	if (newSize == 0)
	{
		kfree(ptr);
		return nullptr;
	}

	Block* header = (Block*)((char*)ptr - sizeof(Block));

	size_t currentTotalSize = bucketSizes[header->order];
	size_t currentDataSize = currentTotalSize - sizeof(Block);

	// New size is smaller than currently-allocated size,
	// we can just return the existing pointer
	if (newSize <= currentDataSize)
	{
		return ptr;
	}

	void* newPtr = kmalloc(newSize);
	// Out of memory
	if (newPtr == nullptr)
	{
		return nullptr;
	}

	size_t copySize = std::min(currentDataSize, newSize);

	// Copy available data
	char* src = (char*)ptr;
	char* dst = (char*)newPtr;
	memcpy(dst, src, copySize);

	kfree(ptr);

	return newPtr;
}

void* operator new(size_t size)
{
	auto ptr = Memory::kmalloc(size);
	if (!ptr)
	{
		panic("Out of memory.");
	}
	return (void*)ptr;
}
void* operator new(size_t size, void* ptr) { return ptr; }

void* operator new[](size_t size)
{
	auto ptr = Memory::kmalloc(size);
	if (!ptr)
	{
		panic("Out of memory.");
	}
	return ptr;
}
void* operator new[](size_t size, void* ptr) { return ptr; }

void operator delete(void* ptr) { Memory::kfree(ptr); }
void operator delete[](void* ptr) { Memory::kfree(ptr); }
void operator delete(void* ptr, size_t size) { Memory::kfree(ptr); }
void operator delete[](void* ptr, size_t size) { Memory::kfree(ptr); }
