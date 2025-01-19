#include <pmm.h>
#include <stdint.h>

#define PAGE_SIZE 0x1000					   // 4KB
#define TABLE_SIZE (sizeof(PageTable))		   // 4KB
#define DIRECTORY_SIZE (sizeof(PageDirectory)) // 4MB
#define PAGE_COUNT 1024
#define TABLE_COUNT 1024

typedef uint32_t TPage;			 // 4B
typedef uint32_t TPageTable;	 // 4KB
typedef uint32_t TPageDirectory; // 4MB

struct PageTable
{
	TPage entries[1024];
} __attribute__((aligned(4096)));

struct PageDirectory
{
	PageTable entries[1024];
} __attribute__((aligned(4096)));

namespace Paging
{
	void init();

	void   map(uint32_t vaddr, uint32_t paddr);
	TPage* getPage(uint32_t vaddr);
	TPage* createPage(uint32_t address);

	constexpr uint32_t getPageDirectoryIndex(uint32_t vaddr);
	constexpr uint32_t getPageTableIndex(uint32_t vaddr);
	constexpr uint32_t getPhysicalAddress(uint32_t vaddr);

	uint32_t getCurrentPageDirectory();
	void	 setCurrentPageDirectory(uint32_t* address);
} // namespace Paging