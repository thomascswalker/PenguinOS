/*

Paging directory memory is located at:
[PD .. PD + 4096 + (1024 * 4096)] (+4096 is the size of the PDEs)
[0x110000 .. 0x511000] == 4MB + 4KB of memory space

Paging Memory
┌─────┬─────┬─────┬─────┬────────┐
│ PD  │ PT0 │ PT1 │ ... │ PT1023 │
└─────┴─────┴─────┴─────┴────────┘

Page Directory (first 4K bytes)
┌──────┬──────┬──────┬──────┬─────────┐
│ PDE0 │ PDE1 │ PDE2 │ ...  │ PDE1023 │
└──────┴──────┴──────┴──────┴─────────┘

Page Directory Entry (4 bytes)
See the PDE enum for details.
┌────────────┬────┬───┬───┬────┬───┬────┬────┬───┐
│ PT Address │ PS │ D │ A │ NC │ W │ US │ RW │ P │
└────────────┴────┴───┴───┴────┴───┴────┴────┴───┘
31           8    7   6   5    4   3    2    1   0


Page Table (PD + (N * 4K), 4K bytes long)
┌──────┬──────┬──────┬─────┬─────────┐
│ PTE0 │ PTE1 │ PTE2 │ ... │ PTE1023 │
└──────┴──────┴──────┴─────┴─────────┘

Page Table Entry (4 bytes)
See the PTE enum for details.
┌───────────┬───┐
│ P Address │ A │
└───────────┴───┘
31          1   0

*/

#include <cstdio.h>
#include <math.h> // IWYU pragma: keep
#include <memory.h>

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

// Free list for each bucket (order).
static Block* freeList[BUCKET_COUNT];

// The page directory is constructed at an arbitrary location, but
// most notably it is REQUIRED to be page-aligned (aligned to 4096).
// 0x110000 is the (likely) location in memory.
static uint32_t pageDirectory[1024] __attribute__((aligned(PAGE_SIZE)));

/**
 * Initializes the memory management system.
 *
 * Sets up the page directory, identity maps the first 8MB of memory,
 * and sets up the buddy allocator for memory allocation.
 *
 * @param start The starting address of the memory region to be managed.
 * @param size The size of the memory region to be managed.
 */
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
		uint32_t  address = PAGE_ALIGN((uintptr_t)pageDirectory + PAGE_SIZE + (i * PAGE_SIZE));
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
	memoryStart = (uintptr_t)pageDirectory + PAGE_SIZE + DIR_SIZE;
	memoryEnd = PAGE_ALIGN(memoryStart + (size - PAGE_SIZE - DIR_SIZE));
	memorySize = memoryEnd - memoryStart;
	memoryPool = (uint8_t*)(uintptr_t)memoryStart;

	// Initialize buddy allocator
	size_t offset = 0;
	for (uint32_t order = BUCKET_COUNT - 1; order > 0; order--)
	{
		// Create blocks of the current order until we run out of memory.
		size_t blockSize = bucketSizes[order];
		while (offset + blockSize < memorySize)
		{
			// Create a new block and add it to the free list.
			Block* block = (Block*)((uint8_t*)memoryPool + offset);
			block->order = order;
			block->isFree = true;
			block->next = nullptr;

			addBlock(block);
			offset += blockSize;
		}
	}
}

/**
 * Identity maps a page table at the given index in the page
 * directory.
 *
 * Sets the specified page table to be present and maps the
 * physical addresses to the same virtual addresses
 * (identity mapping).
 *
 * @param index The index in the page directory where the
 * page table is located.
 */
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

/**
 * Retrieves the page table from a given physical address.
 */
uint32_t* Memory::getTableFromAddress(uint32_t address)
{
	return (uint32_t*)((uintptr_t)pageDirectory[PD_INDEX(address)] & PAGE_MASK);
}

/**
 * Retrieves the page table from a given index in the page
 * directory.
 */
uint32_t* Memory::getTableFromIndex(uint32_t index)
{
	return (uint32_t*)((uintptr_t)pageDirectory[index] & PAGE_MASK);
}

/**
 * Enables paging by setting the appropriate bit in the CR0
 * register.
 */
void Memory::enablePaging()
{
	uint32_t cr0;
	asm("mov %%cr0, %0" : "=r"(cr0));
	cr0 |= 0x80000001; // Paging + Protected Mode
	asm("mov %0, %%cr0" ::"r"(cr0));
}

/**
 * Sets the page directory by writing the address of the
 * page directory to the CR3 register.
 */
void Memory::setPageDirectory(uint32_t* directory)
{
	asm("mov %0, %%cr3" ::"r"((uintptr_t)directory));
}

/**
 * Sets the large paging bit in the CR4 register.
 */
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

/**
 * Dumps the page table information to the terminal.
 */
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

/**
 * Adds a block to the free list for the given order.
 *
 * @param block The block to be added.
 */
void Memory::addBlock(Block* block)
{
	int order = block->order;
	block->next = freeList[order];
	freeList[order] = block;
}

/**
 * Removes a block from the free list for the given order.
 *
 * @param block The block to be removed.
 * @param order The order of the block to be removed.
 */
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

/**
 * Allocates a block of memory of the given size using the
 * buddy allocator.
 *
 * @param size The size of the memory block to allocate.
 *
 * @return A pointer to the allocated memory or nullptr if
 * allocation fails.
 */
void* Memory::kmalloc(size_t size)
{
	// Return nullptr if size is 0.
	if (size == 0)
	{
		return nullptr;
	}

	uint32_t order;
	size_t	 totalSize = size + sizeof(Block);
	// Find the smallest order that can fit the requested
	// size.
	for (order = 0; order < BUCKET_COUNT; order++)
	{
		if (bucketSizes[order] >= totalSize)
		{
			break;
		}
	}

	// If the order is greater than the maximum order,
	// return nullptr.
	if (order > MAX_ORDER)
	{
		warning("kmalloc: Order %d is greater than maximum order %d.", order, MAX_ORDER);
		return nullptr;
	}

	// Find the first free block in the free list starting
	// from the requested order.
	uint32_t currentOrder = order;
	while (currentOrder < BUCKET_COUNT && freeList[currentOrder] == nullptr)
	{
		currentOrder++;
	}

	// If we couldn't find a free block, return nullptr.
	if (currentOrder >= BUCKET_COUNT)
	{
		warning("kmalloc: Unable to find free block (Order %d >= %d).", currentOrder, BUCKET_COUNT);
		return nullptr;
	}

	// We found a free block, so remove it from the free list.
	Block* block = freeList[currentOrder];
	freeList[currentOrder] = block->next;
	block->next = nullptr;

	// If the block we found is larger than the requested order,
	// we need to split it into smaller blocks until we reach the
	// requested order.
	while (currentOrder > order)
	{
		currentOrder--;

		size_t newBlockSize = bucketSizes[currentOrder];

		// Create a new block for the buddy.
		auto buddy = (Block*)((uint8_t*)block + newBlockSize);
		buddy->order = currentOrder;
		buddy->isFree = true;
		buddy->next = nullptr;

		Memory::addBlock(buddy);

		block->order = currentOrder;
	}

	// Mark the block as allocated.
	block->isFree = false;

	// Return a pointer to the memory after the block
	// header.
	return (void*)((uint8_t*)block + sizeof(Block));
}

/**
 * Frees a block of memory allocated by kmalloc.
 *
 * @param ptr The pointer to the memory block to be freed.
 */
void Memory::kfree(void* ptr)
{
	// If the pointer is null, return.
	if (ptr == nullptr)
	{
		return;
	}

	// Get the block header the pointer.
	auto block = (Block*)((char*)ptr - sizeof(Block));

	// Mark the block as free.
	block->isFree = true;

	// Try to merge the block with its buddy if possible.
	// Start with the current order of the block and try to
	// merge
	uint32_t order = block->order;
	while (order < BUCKET_COUNT)
	{
		// Calculate the buddy's address and check if it's free.
		auto   blockOffset = (uintptr_t)((char*)block - (char*)memoryPool);
		size_t blockSize = bucketSizes[order];

		// Calculate the buddy's offset.
		uintptr_t buddyOffset = blockOffset ^ blockSize;
		if (buddyOffset + blockSize > memorySize)
		{
			break; // Buddy is out of bounds
		}

		// Get the buddy block and check if it's free and
		// of the same order.
		Block* buddy = (Block*)((char*)memoryPool + buddyOffset);
		if (!buddy->isFree || buddy->order != order)
		{
			break; // Cannot merge
		}

		// Remove the buddy from the free list.
		Memory::removeBlock(buddy, order);

		// Merge the blocks by taking the lower address block.
		// This ensures that we always merge in the same direction.
		if (buddyOffset < blockOffset)
		{
			block = buddy;
			blockOffset = buddyOffset;
		}

		// Increase the order of the merged block.
		order++;
		// Set the block's new order.
		block->order = order;
	}

	// Add the block back to the free list.
	Memory::addBlock(block);
}

/**
 * Reallocates a block of memory to a new size.
 * If the new size is smaller than the current size,
 * the existing pointer is returned. If the new size
 * is larger, a new block is allocated and the data
 * is copied over and the old block is freed.
 *
 * @param ptr The pointer to the memory block to be
 * reallocated.
 * @param newSize The new size of the memory block.
 */
void* Memory::krealloc(void* ptr, size_t newSize)
{
	// If the pointer is null, just allocate a new block.
	if (ptr == nullptr)
	{
		return kmalloc(newSize);
	}

	// If the new size is 0, free the block and return
	// nullptr.
	if (newSize == 0)
	{
		kfree(ptr);
		return nullptr;
	}

	// Get the block header from the pointer.
	Block* header = (Block*)((char*)ptr - sizeof(Block));

	// Get the current size of the block.
	size_t currentTotalSize = bucketSizes[header->order];
	size_t currentDataSize = currentTotalSize - sizeof(Block);

	// New size is smaller than currently-allocated size,
	// we can just return the existing pointer.
	if (newSize <= currentDataSize)
	{
		return ptr;
	}

	// New size is larger than the currently-allocated size,
	// we need to allocate a new block and copy the data
	// over.
	void* newPtr = kmalloc(newSize);

	// Out of memory
	if (newPtr == nullptr)
	{
		return nullptr;
	}

	// We can only copy the amount of data that will fit
	// in the new block.
	size_t copySize = std::min(currentDataSize, newSize);

	// Copy available data
	char* src = (char*)ptr;
	char* dst = (char*)newPtr;
	memcpy(dst, src, copySize);

	// Free the old block.
	kfree(ptr);

	// Return the new pointer.
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
