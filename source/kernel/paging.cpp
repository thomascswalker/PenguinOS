#include <assert.h>
#include <bitarray.h>
#include <paging.h>
#include <stdio.h>

EXTERN uint32_t kernelStart;
EXTERN uint32_t kernelEnd;

#define PD_INDEX(address) (address >> 22)
#define PT_INDEX(address) ((address >> 12) & 0x3FF)

#define FRAME_MAP_SIZE 0x400000
static std::BitArray<uint32_t, FRAME_MAP_SIZE> bitmap; // 4194304 bits, 524KB

static uint32_t pageDirectory[TABLE_COUNT] __attribute__((aligned(4096)));			// 4KB
static uint32_t pageTables[TABLE_COUNT][PAGE_COUNT] __attribute__((aligned(4096))); // 4MB

static uint32_t nextFreePage = 0;

/*
Initialize paging. Paging is already enabled in boot.s.
*/
void Paging::init()
{
	bitmap = std::BitArray<uint32_t, FRAME_MAP_SIZE>();

	// Zero page directory entries
	for (uint32_t i = 0; i < TABLE_COUNT; i++)
	{
		pageDirectory[i] = 0;
	}
	bitmap.set(0); // The first bit in the bitmap corresponds to the page directory itself

	// Set up identity mapping (physical == virtual) for all 1024 page tables
	for (uint32_t i = 0; i < TABLE_COUNT; i++)
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
	// Map kernel (0xC0000000..0xC03FFFFF) to the physical kernel space (0x100000..0x4FFFFF).
	map(0xC0000000, 0x100000);
}

void Paging::map(uint32_t vaddr, uint32_t paddr)
{
	info("Mapping %x to %x.", vaddr, paddr);
	uint32_t  pdIndex = PD_INDEX(vaddr);
	uint32_t* table = (uint32_t*)pageDirectory[pdIndex];

	// Map all pages within this table
	for (uint32_t i = 0; i < PAGE_COUNT; i++)
	{
		auto address = paddr + (i * PAGE_SIZE); // Page address
		table[i] = address | Present | ReadWrite;
	}
	pageDirectory[pdIndex] = (uint32_t)table | Present | ReadWrite;
}

void* Paging::allocatePage()
{
	// TODO: Test 32 bits at once
	for (uint32_t i = 0; i < bitmap.bitSize(); i++)
	{
		if (!bitmap.test(i))
		{
			bitmap.set(i);
			return (void*)(i * PAGE_SIZE);
		}
	}
	return nullptr;
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
