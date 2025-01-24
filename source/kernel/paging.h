#pragma once

#include <bitarray.h>
#include <stdint.h>

#define PAGE_SIZE 4096
#define PAGE_COUNT 1024
#define TABLE_COUNT 1024
#define FRAME_MAP_SIZE (TABLE_COUNT * PAGE_COUNT * sizeof(uint32_t))
#define KMALLOC_MEMORY_ADDRESS 0x500000

typedef BitArray<uint32_t, FRAME_MAP_SIZE> PageMap;

namespace Paging
{
	enum PageFlag
	{
		Present = (1 << 0),
		ReadWrite = (1 << 1),
	};

	void	 init();
	void	 map(uint32_t vaddr, uint32_t paddr);
	void*	 allocatePage();
	PageMap* getPageMap();
	void	 invalidate(uint32_t vaddr);

	uint32_t getPageDirectoryIndex(uint32_t address);
	uint32_t getPageTableIndex(uint32_t address);

	void setPageDirectory(uint32_t* directory);
	void enablePaging();
} // namespace Paging