#include <assert.h>
#include <bitarray.h>
#include <paging.h>
#include <stdio.h>

EXTERN uint32_t kernelStart;
EXTERN uint32_t kernelEnd;

static uint32_t pageDirectory[TABLE_COUNT] __attribute__((aligned(4096)));			// 4KB
static uint32_t pageTables[TABLE_COUNT][PAGE_COUNT] __attribute__((aligned(4096))); // 4MB

/*
Initialize paging. Paging is already enabled in boot.s.
*/
void Paging::init()
{
	debug("Kernel start: %x, end: %x", &kernelStart, &kernelEnd);
	uint32_t pageDirectorySize = 1024 * 1024 * 4;
	uint32_t pageDirectoryEnd = (uint32_t)pageTables + (pageDirectorySize / 4);
	debug("Initializng Page Directory [%x => %x] (%dB)", pageDirectory, pageDirectoryEnd,
		pageDirectorySize);

	// Zero page directory entries
	for (uint32_t i = 0; i < TABLE_COUNT; i++)
	{
		pageDirectory[i] = 0;
	}

	// Set up identity mapping (physical == virtual) for the first 16MB of memory
	for (uint32_t i = 0; i < 1024; i++)
	{
		uint32_t* table = (uint32_t*)&pageTables[i];
		for (uint32_t entry = 0; entry < PAGE_COUNT; entry++)
		{
			uint32_t address = (i * PAGE_COUNT * PAGE_SIZE) + (entry * PAGE_SIZE);
			table[entry] = address | Present | ReadWrite;
		}
		// Point page directory to the table
		pageDirectory[i] = ((uint32_t)table) | Present | ReadWrite;
	}

	// Enable paging
	setPageDirectory(pageDirectory);
	enablePaging();

	// Once paging is enabled:
	// Map kernel [0xC0000000..0xC7DFFFFF] to the physical kernel space [0x100000..0x7CFFFFFF].
	// This will map 32MB for the kernel to work with.
	// uint32_t vaddr = ALIGN(&kernelStart, 4096) - 0x1000; // Should be 0xC0000000
	// uint32_t paddr = vaddr - 0xC0000000;
	// for (uint32_t i = 0; i < 32; i++)
	// {
	// 	uint32_t offset = i * MB(4);
	// 	map(vaddr + offset, paddr + offset);
	// }

	success("Paging enabled.");
}

/*
Maps 4MB from [vaddr..vaddr + 4mb] to [paddr..paddr + 4mb].
*/
void Paging::map(uint32_t vaddr, uint32_t paddr)
{
	debug("Mapping %x to %x.", vaddr, paddr - 1);
	uint32_t  pdIndex = getDirectoryIndex(vaddr);
	uint32_t* table = (uint32_t*)pageDirectory[pdIndex];

	// Map all pages within this table
	for (uint32_t i = 0; i < PAGE_COUNT; i++)
	{
		auto address = paddr + (i * PAGE_SIZE); // Page address
		table[i] = address | Present | ReadWrite;
	}
	pageDirectory[pdIndex] = (uint32_t)table | Present | ReadWrite;
}

void Paging::invalidate(uint32_t vaddr)
{
	asm("invlpg %0" : : "m"(vaddr));
}

void Paging::allocate(uint32_t* page, PageFlag flags)
{
}

void Paging::free(uint32_t* page)
{
	resetFlag(page, Present);
}

constexpr void Paging::setFlag(uint32_t* entry, PageFlag flag)
{
	*entry |= flag;
}

constexpr void Paging::resetFlag(uint32_t* entry, PageFlag flag)
{
	*entry &= ~(flag);
}

constexpr bool Paging::testFlag(uint32_t* entry, PageFlag flag)
{
	return (*entry & flag) == flag;
}

uint32_t Paging::getDirectoryIndex(uint32_t address)
{
	return address >> 22;
}

uint32_t Paging::getTableIndex(uint32_t address)
{
	return (address >> 12) & 0x3FF;
}

uint32_t Paging::getAddress(uint32_t* page)
{
	return *page & 0xFFFFF000;
}

void Paging::setPageDirectory(uint32_t* directory)
{
	asm("mov %0, %%cr3" ::"r"(directory));
}

void Paging::enablePaging()
{
	// Read the current value of CR0
	uint32_t cr0;
	asm("mov %%cr0, %0" : "=r"(cr0));

	// Paging is enabled by the setting the highest bit to 1.
	// Protected mode is enabled by setting the lowest bit to 1.
	cr0 |= 0x80000001;

	// Enable paging (set PG and PE bits in CR0)
	asm("mov %0, %%cr0" ::"r"(cr0));
}
