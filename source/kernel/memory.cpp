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

EXTERN uint32_t kernelStart;
EXTERN uint32_t kernelEnd;

static uint32_t memoryStart;
static uint32_t memoryEnd;
static uint32_t memorySize;

static Block* blocks;

// The page directory is constructed at an arbitrary location, but
// most notably it is REQUIRED to be page-aligned (aligned to 4096).
// 0x110000 is the (likely) location in memory.
static uint32_t pageDirectory[1024] __attribute__((aligned(PAGE_SIZE)));

void Memory::init(uint32_t start, uint32_t size)
{
	// Zero out all entries in the page directory.
	memset(pageDirectory, 0, sizeof(PageDirectory));

	debug("Initializing page directory...");
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
	debug("Initializing physical memory allocation...");
	memoryStart = (uint32_t)pageDirectory + 0x1000 + DIR_SIZE;
	memoryEnd = PAGE_ALIGN(memoryStart + (size - 0x1000 - DIR_SIZE));
	memorySize = memoryEnd - memoryStart;
	debug("Physical memory area is from [%x => %x] (%xB)", memoryStart, memoryEnd, memorySize);

	uint32_t blockByteCount = ceildiv(PAGE_ALIGN(memorySize), BLOCK_SIZE);
	uint32_t blockCount = blockByteCount * 8;
	blocks = (Block*)memoryStart;
	uint32_t blockMemorySize = PAGE_ALIGN(blockByteCount) * sizeof(Block);
	debug("Allocating %dKB for block memory at %x.", blockMemorySize / 1024, blocks);
	memset(blocks, 0, blockMemorySize);
}

void Memory::identityMapTable(uint32_t index)
{
	// Set this table to present.
	uint32_t* entry = &pageDirectory[index];
	*entry |= PDE_Present;

	// Because we're identity mapping (physical == virtual),
	// we can just unshift the table index to retrieve
	// the address this table is mapped to.
	uint32_t   offset = index << 22;
	PageTable* table = getTableFromIndex(index);
	for (uint32_t i = 0; i < PAGE_COUNT; i++)
	{
		table[i] = offset + (i * PAGE_SIZE) | 0x1; // Active page
	}
}

PageTable* Memory::getTableFromAddress(uint32_t address)
{
	return (PageTable*)(pageDirectory[PD_INDEX(address)] & PAGE_MASK);
}

PageTable* Memory::getTableFromIndex(uint32_t index)
{
	return (PageTable*)(pageDirectory[index] & PAGE_MASK);
}

void Memory::enablePaging()
{
	debug("Enabling paging.");
	uint32_t cr0;
	asm("mov %%cr0, %0" : "=r"(cr0));
	cr0 |= 0x80000001; // Paging + Protected Mode
	asm("mov %0, %%cr0" ::"r"(cr0));
	success("Paging enabled.");
}

void Memory::setPageDirectory(PageDirectory* directory)
{
	debug("Setting page directory to %x.", directory);
	asm("mov %0, %%cr3" ::"r"((uint32_t)directory));
}

void Memory::setLargePaging(bool state)
{
	uint32_t cr4;
	asm("mov %%cr4, %0" : "=r"(cr4));
	if (state)
	{
		debug("Enabling large paging.");
		cr4 |= 0x00000010;
	}
	else
	{
		debug("Disabling large paging.");
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
		PageTable* table = getTableFromIndex(i);
		uint32_t   start = table[0] & PAGE_MASK;
		uint32_t   end = (table[1023] & PAGE_MASK) + 0x1000 - 1;
		printf("Table %d: %x => %x\n", i, start, end);
	}
}
