#include <memory.h>

void Paging::init(uint32_t size)
{
	debug("Enabling paging...");

	debug("Physical memory size: %d.", size);
	uint32_t maxPages = size / PAGE_SIZE; // 4096 pages
	debug("Page count: %d.", maxPages);
	debug("PageDir size: %d", sizeof(TPageDirectory));

	TPageDirectory* tmp = pageDirectory;
	tmp = (TPageDirectory*)_kmalloc(sizeof(TPageDirectory));
	debug("Initial kmalloc: %x", (uint32_t)tmp);

	memset(tmp, 0, sizeof(TPageDirectory));

	uint32_t i = 0xC0000000;
	while (i < 0xC0000000 + (4 * 0x100000))
	{
		allocatePage(tmp, i, 0, 1, 1);
		i += PAGE_SIZE;
	}

	// for (uint32_t i = 0; i < TABLES_PER_DIRECTORY; i++)
	// {
	// 	TPageTable* table = &pageDirectory[i];
	// 	*table &= ~Present;
	// 	*table |= ReadWrite;
	// 	*table &= ~UserSupervisor;
	// }

	// for (uint32_t i = 0; i < PAGES_PER_TABLE; i++)
	// {
	// 	TPage* page = &pageTable[i];
	// 	*page = i * PAGE_SIZE;		  // Set address
	// 	*page |= Present | ReadWrite; // Set flags
	// }

	// debug("Table0 = %d, %x", pageDirectory[0], &pageDirectory[0]);
	// pageDirectory[0] = ((uint32_t)pageTable) | Present | ReadWrite;
	// debug("Table0 = %x, %x", pageDirectory[0] >> 22, &pageDirectory[0]);
	// success("Paging enabled.");
}

void Paging::allocatePage(
	TPageDirectory* dir, uint32_t vaddr, uint32_t frame, bool isKernel, bool isWritable)
{
}

void* Paging::_kmalloc(uint32_t size)
{
	void* ret = mallocMemory;
	if (!IS_ALIGN(ret))
	{
		ret = (void*)PAGE_ALIGN(ret);
	}
	mallocMemory += size;
	return ret;
}
