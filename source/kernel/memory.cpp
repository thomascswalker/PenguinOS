#include <memory.h>
#include <stdio.h>

EXTERN uint32_t kernelStart;
EXTERN uint32_t kernelEnd;
static uint32_t memoryStart;
static uint32_t memoryEnd;

// The page directory is constructed at an arbitrary location, but
// most notably it is REQUIRED to be page-aligned (aligned to 4096).
static uint32_t pageDirectory[1024] __attribute__((aligned(PAGE_SIZE)));

void Memory::init(uint32_t start, uint32_t size)
{
	// Page-aligned start and end of usable memory in the system.
	memoryStart = PAGE_ALIGN(start);
	memoryEnd = PAGE_ALIGN(start + size);

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

	// Identity map the first 4MB of memory
	// 0x0 => 0x3FFFFF
	identityMapTable(0);

	// Identity map the 768th table (at 0xC0000000)
	// 0xC0000000 => 0xC003FFFFF
	// 0xC0000000 >> 22 == 768
	uint32_t table768 = PD_INDEX(VIRTUAL_START);
	identityMapTable(table768);

	setPageDirectory(pageDirectory);

	// Disable 4MB paging.
	setLargePaging(false);
	enablePaging();
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
	return (PageTable*)(GET_ADDRESS(pageDirectory[PD_INDEX(address)]));
}

PageTable* Memory::getTableFromIndex(uint32_t index)
{
	return (PageTable*)(GET_ADDRESS(pageDirectory[index]));
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
