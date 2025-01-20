#include <assert.h>
#include <paging.h>
#include <stdio.h>

EXTERN uint32_t	 pagingStart;							  // 0x101000
static uint32_t* pagingEnd;								  // 0x102000, 4MB long
static uint32_t* pageDirectory = (uint32_t*)&pagingStart; // 0x101000

/*
Initialize paging. Paging is already enabled in boot.s.
*/
void Paging::init()
{
	// The page directory ends 4MB (size of table entries) + 4KB
	// (size of directory entries) from the start of the directory.
	pagingEnd = &pagingStart + 0x1000;
	debug("Page directory is from %x to %x (%dB).", &pagingStart, pagingEnd,
		pagingEnd - &pagingStart);

	// Set all entries in the page directory to ReadWrite,
	// but not present. As we allocate tables they will
	// become present.
	debug("Setting all page directory entries to [ReadWrite].");
	memset(pageDirectory, PDE_ReadWrite, 1024);

	// Identity map the number of pages enough to cover
	// the size of the kernel. This maps the kernel to itself
	// so any memory access of the kernel does not throw a
	// page fault exception.
	uint32_t kernelSize = &kernelEnd - &kernelStart;
	uint32_t kernelPageCount = ceildiv(kernelSize, PAGE_SIZE);
	debug("Identity mapping %d pages for kernel size %d bytes.", kernelPageCount, kernelSize);
	for (uint32_t i = 0; i < kernelPageCount; i++)
	{
		VirtualAddress address = (uint32_t)&kernelStart + (i * PAGE_SIZE);
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

	// Get the Page Directory and Page Table indexes
	// stored in this virtual address.
	uint32_t pdIndex = getPageDirectoryIndex(vaddr);
	uint32_t ptIndex = getPageTableIndex(vaddr);

	uint32_t* pdEntry = &pageDirectory[pdIndex];
	uint32_t  ptAddress = *pdEntry;
	uint32_t* table = (uint32_t*)ptAddress;

	// Is there a corresponding page table yet?
	// If no, create a new table (allocating a block)
	// and set the corresponding index in the page directory
	// to the table.
	if (!hasFlag(pdEntry, PDE_Present))
	{
		debug("Table %d not found. Making new table.", pdIndex);
		table = createPageTable(pdEntry);
	}
	// Otherwise just get the existing table.
	else
	{
		table = (uint32_t*)&pageDirectory[pdIndex];
		debug("Table %d found at %x.", pdIndex, table);
	}

	TPage* page = &table[ptIndex];
	*page |= PTE_Present; // Present
	setPhysicalAddress(page, paddr);
	debug("Set [%d][%d] to %x.", pdIndex, ptIndex, *page);
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

void Paging::setPhysicalAddress(uint32_t* entry, uint32_t address)
{
	*entry = (*entry & ~0xFFFFF000) | address;
}

uint32_t* Paging::createPageTable(uint32_t* pdEntry)
{
	auto allocator = PMM::getAllocator();

	// Allocate 1 block
	uint32_t* table = allocator->allocate(1);
	debug("Allocated new table at %x.", table);

	// Zero out the memory for the table
	memset(table, 0, 1024);

	// Set the flags for the directory entry associated
	// with this table to Present and ReadWrite.
	setFlags(pdEntry, PDE_Present | PDE_ReadWrite);

	// Set the physical address pointed to by the directory
	// entry to the address of the table we just made.
	setPhysicalAddress(pdEntry, (uint32_t)table);

	return table;
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

uint32_t Paging::getFlags(VirtualAddress* address)
{
	return *address & ~(0xFFFFF000);
}

constexpr bool Paging::hasFlag(VirtualAddress* address, uint8_t flag)
{
	return (*address & flag) == flag;
}

void Paging::setFlags(VirtualAddress* address, uint8_t flag)
{
	*address |= flag;
}

void Paging::resetFlags(VirtualAddress* address, uint8_t flag)
{
	*address &= ~flag;
}
