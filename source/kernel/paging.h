#pragma once

#include <bitarray.h>
#include <stdint.h>

#define PAGE_SIZE 4096
#define PAGE_COUNT 1024
#define TABLE_COUNT 1024
#define FRAME_MAP_SIZE (TABLE_COUNT * PAGE_COUNT * sizeof(uint32_t))
#define KMALLOC_MEMORY_ADDRESS 0x500000

typedef BitArray<uint32_t> PageMap;

namespace Paging
{
	typedef uint32_t Page;

	struct PageTable
	{
		uint32_t pages[PAGE_COUNT];
	};

	struct PageDirectory
	{
		uint32_t tables[TABLE_COUNT];
	};

	enum PageFlag
	{
		Present = (1 << 0),
		ReadWrite = (1 << 1),
		User = (1 << 2)
	};

	void init();
	void map(uint32_t vaddr, uint32_t paddr);
	void invalidate(uint32_t vaddr);
	void allocate(uint32_t* page, PageFlag flags);
	void free(uint32_t* page);

	constexpr void setFlag(uint32_t* entry, PageFlag flag);
	constexpr void resetFlag(uint32_t* entry, PageFlag flag);
	constexpr bool testFlag(uint32_t* entry, PageFlag flag);

	uint32_t getDirectoryIndex(uint32_t address);
	uint32_t getTableIndex(uint32_t address);
	uint32_t getAddress(uint32_t* page);

	void setPageDirectory(uint32_t* directory);
	void enablePaging();
} // namespace Paging