#pragma once

#include <bitmask.h>
#include <pmm.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define VIRTUAL_START 0xC0000000 // 3GB
#define PHYSICAL_START 0X100000	 // 1MB
#define PHYSICAL_SIZE 0x1000000	 // 16MB

#define PAGE_SIZE 4096 // 4KB
#define PAGES_PER_TABLE 1024
#define TABLES_PER_DIRECTORY 1024

#define IS_ALIGN(addr) ((((uint32_t)(addr)) | 0xFFFFF000) == 0)
#define PAGE_ALIGN(addr) ((((uint32_t)(addr)) & 0xFFFFF000) + 0x1000)

struct PageTableEntry;
struct PageDirectoryEntry;

enum PageFlag : uint8_t // Page Table Flags
{
	Present = (1 << 0),		   // Page is actually in physical memory
	ReadWrite = (1 << 1),	   // The page is read/write, otherwise read-only
	UserSupervisor = (1 << 2), // Page can be accessed by all, otherwise only supervisor access
	WriteThrough = (1 << 3),   // Write through caching is enabled
	CacheDisable = (1 << 4),   // Page will not be cached
	Accessed = (1 << 5),	   // Read during virtual address translation
	Dirty = (1 << 6),		   // Whether the page has been written to
	PageSize = (1 << 7)		   // 4MB in size, otherwise 4KB
};
DEFINE_BITMASK_OPERATORS(PageFlag);

static bool getFlag(uint32_t entry, PageFlag f)
{
	return entry & f;
}
static void setFlag(uint32_t& entry, PageFlag f, bool state)
{
	state ? entry |= f : entry &= ~f;
}

typedef uint32_t TPage;
typedef uint32_t TPageTable;
typedef uint32_t TPageDirectory;

namespace Paging
{
	// PageDirectory  kernelDirectory;
	// PageDirectory* currentDirectory;
	// PageTable*	   firstTable = nullptr;

	static TPageTable pageDirectory[TABLES_PER_DIRECTORY];
	static TPage	  pageTable[PAGES_PER_TABLE];

	static TPageTable* currentTable;
	static TPage*	   currentPage;

	static uint8_t* mallocMemory;

	EXTERN void enablePaging(); // Defined in boot.s

	void init(uint32_t size);
	void allocatePage(
		TPageDirectory* dir, uint32_t vaddr, uint32_t frame, bool isKernel, bool isWritable);

	void* _kmalloc(uint32_t size);

} // namespace Paging