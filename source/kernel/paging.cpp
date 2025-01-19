#include <assert.h>
#include <paging.h>
#include <stdio.h>

EXTERN uint32_t pagingStart;
static uint32_t pagingEnd;

static uint32_t* pageDirectory = (uint32_t*)&pagingStart;
static uint32_t* pageTable = (uint32_t*)(&pagingStart + 0x1000);

/*
Initialize paging. Paging is already enabled in boot.s.
*/
void Paging::init()
{
	debug("Page directory is at %x.", pageDirectory);

	// Set the first entry in the page directory to our page table
	pageDirectory[0] = (uint32_t)pageTable | 0x83;
	debugx(pageTable);

	// For each entry in our page table, map it to itself
	// (identity map).
	for (uint32_t i = 0; i < PAGE_COUNT; i++)
	{
		uint32_t address = i * PAGE_SIZE;
		map(address, address);
	}
}

void Paging::map(uint32_t vaddr, uint32_t paddr)
{
	uint32_t pdIndex = getPageDirectoryIndex(vaddr);
	uint32_t ptIndex = getPageTableIndex(vaddr);

	// TODO: Implement something for multiple tables
	uint32_t* table = pageTable;
	uint32_t* page = &table[ptIndex];
	debug("Setting PT[%d][%d] to %x.", pdIndex, ptIndex, paddr | 0x03);
	*page = paddr | 0x03;
}

TPage* Paging::getPage(uint32_t vaddr)
{
	return nullptr;
}

TPage* Paging::createPage(uint32_t address)
{
	return nullptr;
}

constexpr uint32_t Paging::getPageDirectoryIndex(uint32_t vaddr)
{
	return (vaddr >> 22);
}

constexpr uint32_t Paging::getPageTableIndex(uint32_t vaddr)
{
	return ((vaddr >> 12) & 0x3FF);
}

constexpr uint32_t Paging::getPhysicalAddress(uint32_t vaddr)
{
	return (vaddr & 0xFFF);
}

/*
	Returns the value of the `CR3` register, which contains
	the address of the current page directory.
*/
uint32_t Paging::getCurrentPageDirectory()
{
	uint32_t out;
	asm("mov %%cr3, %0" : "=r"(out));
	return out;
}

/*
	Sets the value of `CR3` to point to the new address
	of the current page directory.
*/
void Paging::setCurrentPageDirectory(uint32_t* address)
{
	debug("Setting current page directory to %x.", address);
	asm("mov %0, %%cr3" ::"a"(address));
}
