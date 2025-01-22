#include <stdint.h>

#define PAGE_SIZE 0x1000			   // 4KB
#define TABLE_SIZE 0x1000			   // 4KB
#define DIRECTORY_SIZE 0x1000 * 0x1000 // 4MB
#define PAGE_COUNT 1024
#define TABLE_COUNT 1024

namespace Paging
{
	enum PageFlag
	{
		Present = (1 << 0),
		ReadWrite = (1 << 1),
	};

	void  init();
	void  map(uint32_t vaddr, uint32_t paddr);
	void* allocatePage();

	void setPageDirectory(uint32_t* directory);
	void enablePaging();
} // namespace Paging