#include <assert.h>
#include <paging.h>
#include <stdio.h>

EXTERN uint32_t	 pagingStart; // 0x101000
static uint32_t* pagingEnd;	  // 0x501000, 4MB long

static uint32_t* pageDirectory = (uint32_t*)&pagingStart; // 0x101000
static uint32_t* pageTables = nullptr; // 0x105000, 0x1000 from the start of pageDirectory

/*
Initialize paging. Paging is already enabled in boot.s.
*/
void Paging::init()
{
	// The page directory ends 4MB (size of table entries) + 4KB
	// (size of directory entries) from the start of the directory.
	pagingEnd = &pagingStart + (TABLE_COUNT * PAGE_COUNT * 4) + 0x1000;
	debug("Page directory is from %x to %x (%dB).", &pagingStart, pagingEnd,
		pagingEnd - &pagingStart);
	pageTables = pageDirectory + 0x1000;
	ASSERT(pageDirectory == pageTables - 0x1000);
	debug("Page tables start at %x.", pageTables);

	// Get the first table in the table array
	uint32_t index = getPageTableIndex((uint32_t)&kernelStart);
	debug("Initializing table %d.", index);
	TPageTable* table = getPageTable(index);

	// Set this table's flags to [Present | ReadWrite].
	*table = PTE_Present | PTE_ReadWrite;

	// Set the first entry in the page directory to this table,
	// also setting the flags to [Present | ReadWrite].
	pageDirectory[0] = (uint32_t)table | PDE_Present | PDE_ReadWrite;

	// Identity map the number of pages enough to cover
	// the size of the kernel. This maps the kernel to itself
	// so any memory access of the kernel does not throw a
	// page fault exception.
	uint32_t kernelSize = &kernelEnd - &kernelStart;
	uint32_t kernelPageCount = ceildiv(kernelSize, PAGE_SIZE);
	debug("Identity mapping %d pages.", kernelPageCount);
	for (uint32_t i = 0; i < kernelPageCount; i++)
	{
		PhysicalAddress address = (uint32_t)&kernelStart + (i * PAGE_SIZE);
		map(address, address);
	}
	debug("Identity mapping complete.");
}

void Paging::map(VirtualAddress vaddr, PhysicalAddress paddr)
{
	if (vaddr == paddr)
	{
		debug("Identity mapping %x.", vaddr);
	}
	else
	{
		debug("Mapping %x to %x.", vaddr, paddr);
	}
	TPage* page = getPage(vaddr);
	*page = paddr | 0x1; // Present
}

TPage* Paging::getPage(VirtualAddress vaddr)
{
	uint32_t  pdIndex = getPageDirectoryIndex(vaddr);
	uint32_t  ptIndex = getPageTableIndex(vaddr);
	uint32_t* table = getPageTable(pdIndex);
	return &table[ptIndex];
}

TPageTable* Paging::getPageTable(uint32_t index)
{
	return pageTables + (index * TABLE_SIZE);
}

/*
		Virtual Address
-------------------------------
| 10 bits | 10 bits | 12 bits |
-------------------------------
	^
*/
constexpr uint32_t Paging::getPageDirectoryIndex(VirtualAddress vaddr)
{
	return (vaddr >> 22);
}

/*
		Virtual Address
-------------------------------
| 10 bits | 10 bits | 12 bits |
-------------------------------
			   ^
*/
constexpr uint32_t Paging::getPageTableIndex(VirtualAddress vaddr)
{
	return ((vaddr >> 12) & 0x3FF);
}

/*
		Virtual Address
-------------------------------
| 10 bits | 10 bits | 12 bits |
-------------------------------
						 ^
*/
constexpr uint32_t Paging::getPhysicalAddress(VirtualAddress vaddr)
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
void Paging::setCurrentPageDirectory(PhysicalAddress address)
{
	debug("Setting current page directory to %x.", address);
	asm("mov %0, %%cr3" ::"a"(address));
}

uint32_t Paging::getFlags(VirtualAddress address)
{
	return address & ~(0xFFFFF000);
}
