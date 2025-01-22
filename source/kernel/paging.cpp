#include <assert.h>
#include <bitarray.h>
#include <paging.h>
#include <stdio.h>

EXTERN uint32_t kernelStart;
EXTERN uint32_t kernelEnd;

#define SET_FLAG(entry, f) (*entry |= f)
#define RESET_FLAG(entry, f) (*entry &= ~(f))
#define HAS_FLAG(entry, f) ((*entry & f) == f)
#define SET_ADDRESS(entry, address) (*entry = (*entry & ~0xFFFFF000) | address)

#define PD_INDEX(address) (address >> 22)
#define PT_INDEX(address) ((address >> 12) & 0x3FF)

#define FRAME_MAP_SIZE 0x400000
static std::BitArray<uint32_t, FRAME_MAP_SIZE> bitmap; // 1048576 bits

static uint32_t pageDirectory[1024] __attribute__((aligned(4096)));
static uint32_t pageTables[6][1024] __attribute__((aligned(4096)));

/*
Initialize paging. Paging is already enabled in boot.s.
*/
void Paging::init()
{
	bitmap = std::BitArray<uint32_t, FRAME_MAP_SIZE>();
	debugx(&bitmap);

	// Zero page directory entries
	for (uint32_t i = 0; i < TABLE_COUNT; i++)
	{
		pageDirectory[i] = 0;
	}

	map(pageTables[0], 0, 0);
	map(pageTables[1], 0xC0000000, 0x100000);
	map(pageTables[2], 0xC0100000, 0x200000);
	map(pageTables[3], 0xC0200000, 0x300000);
	map(pageTables[4], 0xC0300000, 0x400000);
	map(pageTables[5], 0xC0400000, 0x500000);

	setPageDirectory(pageDirectory);
	enablePaging();
}

void Paging::map(uint32_t* table, uint32_t vaddr, uint32_t paddr)
{
	debug("Mapping %x to %x.", vaddr, paddr);
	uint32_t pdIndex = PD_INDEX(vaddr);

	for (uint32_t i = 0; i < PAGE_COUNT; i++)
	{
		auto address = paddr + (i * PAGE_SIZE);
		table[i] = address | Present | ReadWrite;
	}
	pageDirectory[pdIndex] = (uint32_t)table | Present | ReadWrite;
}

void Paging::setPageDirectory(uint32_t* directory)
{
	asm("mov %0, %%cr3" ::"r"(directory));
}

void Paging::enablePaging()
{
	// Enable paging (set PG and PE bits in CR0)
	uint32_t cr0;
	asm("mov %%cr0, %0" : "=r"(cr0)); // Read CR0
	cr0 |= 0x80000001;				  // Enable paging (PG = 1) and protected mode (PE = 1)
	asm("mov %0, %%cr0" ::"r"(cr0));
}
