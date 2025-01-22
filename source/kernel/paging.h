#include <stdint.h>

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

	uint32_t getPageDirectoryIndex(uint32_t address);
	uint32_t getPageTableIndex(uint32_t address);

	void setPageDirectory(uint32_t* directory);
	void enablePaging();
} // namespace Paging