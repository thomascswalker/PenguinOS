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

#include <bitarray.h>
#include <math.h>
#include <memory.h>
#include <stdio.h>

EXTERN uint32_t kernelStart;
EXTERN uint32_t kernelEnd;

static uint32_t memoryStart;
static uint32_t memoryEnd;
static uint32_t memorySize;
static void*	mallocStart;

// Singlely linked-list for tracking all in-use blocks
// and their respective sizes.
static Block* blockHead;
static Block* blockTail;

typedef std::BitArray<uint32_t> BlockMap;
// Bit array for tracking whether a block is in use or not.
static BlockMap* blockMap;

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
	enablePaging();

	// Setup memory allocation
	// Page-aligned start and end of usable memory in the system.
	memoryStart = (uint32_t)pageDirectory + PAGE_SIZE + DIR_SIZE;
	memoryEnd = PAGE_ALIGN(memoryStart + (size - PAGE_SIZE - DIR_SIZE));
	memorySize = memoryEnd - memoryStart;

	// Initialize the block linked list.
	uint32_t blockCount = CEILDIV(PAGE_ALIGN(memorySize), BLOCK_SIZE);
	uint32_t blockByteCount = blockCount / 8;
	blockHead = (Block*)memoryStart;
	blockTail = blockHead; // Tail == Head
	uint32_t blockMemorySize = PAGE_ALIGN(blockByteCount) * sizeof(Block);
	memset(blockHead, 0, blockMemorySize);

	// Now that we have the number of blocks and the maximum possible
	// linked list size, we can initialize the block bitmap.
	blockMap = (BlockMap*)((uint32_t)blockHead + blockMemorySize);
	*blockMap = BlockMap(blockByteCount);

	// Set the memory for the block map itself to always be in use.
	for (uint32_t i = 0; i < (uint32_t)CEILDIV(blockByteCount, BLOCK_SIZE); i++)
	{
		blockMap->set(i);
	}

	// Initialize malloc-able memory at the same location of the block map.
	// The block map itself has already been reserved in the above lines
	// so there should be no worry of overlapping block usage.
	mallocStart = (void*)blockMap;
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

bool Memory::allocateBlocks(uint32_t count, int32_t* index)
{
	if (count == 0)
	{
		warning("Invalid 'count' argument: %d.", count);
		return false;
	}
	*index = blockMap->firstContiguous(count);
	if (*index < 0)
	{
		error("Out of memory.");
		return false;
	}
	for (uint32_t i = *index; i < *index + count; i++)
	{
		blockMap->set(i);
	}
	return true;
}

void Memory::freeBlocks(uint32_t index, uint32_t count)
{
	for (uint32_t i = index; i < index + count; i++)
	{
		blockMap->reset(i);
	}
}

uint32_t Memory::getBlockSize(uint32_t v)
{
	v--;
	for (uint32_t i = 0; i < 5; i++)
	{
		v |= v >> (1 << i);
	}
	v++;
	return v >= MAX_BLOCK_SIZE ? MAX_BLOCK_SIZE : v;
}

void* std::kmalloc(uint32_t size)
{
	// Get the block count for the memory size requested.0
	uint32_t blockCount = CEILDIV(size, BLOCK_SIZE);
	int32_t	 index;

	// Attempt to allocate the number of blocks requested.
	// If this fails, we're out of memory. `index` is the
	// in/out parameter to get the index of the first block
	// in the block sequence we allocated.
	if (!Memory::allocateBlocks(blockCount, &index))
	{
		panic("Out of memory!");
		return nullptr;
	}

	// Create a new block `sizeof(Block)` bytes from the current
	// tail of the block list. This will become the new tail.
	Block* newTail = (Block*)((uint32_t)blockTail + sizeof(Block));
	blockTail->next = newTail;
	newTail->size = size;
	newTail->index = index;
	blockTail = newTail;

	// Return a pointer to the offset of the block sequence
	// start index in malloc memory space.
	return (void*)((uint32_t)mallocStart + (index * BLOCK_SIZE));
}

void std::kfree(void* ptr)
{
	uint32_t index = ((uint32_t)ptr - (uint32_t)mallocStart) / BLOCK_SIZE;
	Block*	 current = blockHead;
	while (current->next != nullptr)
	{
		if (current->index == index)
		{
			break;
		}
		current = current->next;
	}
	uint32_t count = CEILDIV(current->size, BLOCK_SIZE);
	Memory::freeBlocks(index, count);
}

void* operator new(size_t size)
{
	auto ptr = std::kmalloc(size);
	if (!ptr)
	{
		panic("Out of memory.");
	}
	return (void*)ptr;
}
void* operator new(size_t size, void* ptr) { return ptr; }

void* operator new[](size_t size)
{
	auto ptr = std::kmalloc(size);
	if (!ptr)
	{
		panic("Out of memory.");
	}
	return ptr;
}
void* operator new[](size_t size, void* ptr) { return ptr; }

void operator delete(void* ptr) { std::kfree(ptr); }
void operator delete[](void* ptr) { std::kfree(ptr); }
void operator delete(void* ptr, size_t size) { std::kfree(ptr); }
void operator delete[](void* ptr, size_t size) { std::kfree(ptr); }
