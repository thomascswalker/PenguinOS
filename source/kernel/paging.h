#include <pmm.h>
#include <stdint.h>

#define PAGE_SIZE 0x1000			   // 4KB
#define TABLE_SIZE 0x1000			   // 4KB
#define DIRECTORY_SIZE 0x1000 * 0x1000 // 4MB
#define PAGE_COUNT 1024
#define TABLE_COUNT 1024

typedef uint32_t TPage;			 // 4B
typedef uint32_t TPageTable;	 // 4KB
typedef uint32_t TPageDirectory; // 4MB

typedef uint32_t VirtualAddress;
typedef uint32_t PhysicalAddress;

enum PageDirectoryFlags
{
	PDE_Present = 0x01,
	PDE_ReadWrite = 0x02,
};

enum PageTableFlags
{
	PTE_Present = 0x01,
	PTE_ReadWrite = 0x02,
};

namespace Paging
{
	void init();

	void map(VirtualAddress vaddr, PhysicalAddress paddr);

	TPage*		getPage(VirtualAddress vaddr);
	TPageTable* getPageTable(uint32_t index);

	constexpr uint32_t getPageDirectoryIndex(VirtualAddress vaddr);
	constexpr uint32_t getPageTableIndex(VirtualAddress vaddr);
	constexpr uint32_t getPhysicalAddress(VirtualAddress vaddr);

	uint32_t getCurrentPageDirectory();
	void	 setCurrentPageDirectory(PhysicalAddress address);

	uint32_t getFlags(VirtualAddress address);
} // namespace Paging