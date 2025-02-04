#pragma once

#include <stddef.h>
#include <stdint.h>

#define BLOCK_SIZE 0x400
#define MAX_BLOCK_SIZE 0x1000
#define BLOCK_MASK 0xFFFFF000
#define VIRTUAL_START 0xC0000000

#define PAGE_SHIFT 12
#define PAGE_SIZE ((uint32_t)(1 << PAGE_SHIFT)) // 4KB
#define PAGE_MASK (~(PAGE_SIZE - 1))
#define PAGE_ALIGN(address) (((address) + PAGE_SIZE - 1) & PAGE_MASK)

#define PAGE_COUNT 1024
#define TABLE_COUNT 1024
#define TABLE_SIZE 0x1000 // 4KB
#define DIR_SIZE 0x400000 // 4MB

#define PD_INDEX(x) (((x) >> 22) & 0x3FF)
#define PT_INDEX(x) (((x) >> 12) & 0x3FF)

#define ENTRY_TEST(e, x) ((*e & x) == x)
#define ENTRY_SET(e, x) (*e |= x)
#define ENTRY_RESET(e, x) (*e &= ~(x))

enum PTE : uint32_t
{
	PTE_Present = (1 << 0),		 // 00000000000000000000000000000001
	PTE_Write = (1 << 1),		 // 00000000000000000000000000000010
	PTE_User = (1 << 2),		 // 00000000000000000000000000000100
	PTE_WriteThrough = (1 << 3), // 00000000000000000000000000001000
	PTE_NonCacheable = (1 << 4), // 00000000000000000000000000010000
	PTE_Accessed = (1 << 5),	 // 00000000000000000000000000100000
	PTE_Dirty = (1 << 6),		 // 00000000000000000000000001000000
	PTE_Reserved = 0x180,		 // 00000000000000000000000110000000
};

enum PDE : uint32_t
{
	PDE_Present = (1 << 0),		 // 00000000000000000000000000000001
	PDE_Write = (1 << 1),		 // 00000000000000000000000000000010
	PDE_User = (1 << 2),		 // 00000000000000000000000000000100
	PDE_WriteThrough = (1 << 3), // 00000000000000000000000000001000
	PDE_NonCacheable = (1 << 4), // 00000000000000000000000000010000
	PDE_Accessed = (1 << 5),	 // 00000000000000000000000000100000
	PDE_Dirty = (1 << 6),		 // 00000000000000000000000001000000
	PDE_PageSize = 0x180,		 // 00000000000000000000000110000000
};

struct Block
{
	uint32_t size;
	uint32_t index;
	Block*	 next;
};

namespace std
{
	void* kmalloc(uint32_t size);
	void  kfree(void* ptr);
} // namespace std

namespace Memory
{
	void init(uint32_t start, uint32_t size);

	/* Paging */

	void identityMapTable(uint32_t index);

	uint32_t* getTableFromAddress(uint32_t address);
	uint32_t* getTableFromIndex(uint32_t index);

	void enablePaging();
	void setPageDirectory(uint32_t* directory);
	void setLargePaging(bool state);

	void dumpPageTable();

	/* Memory Allocation */

	bool	 allocateBlocks(uint32_t count, int32_t* index);
	void	 freeBlocks(uint32_t index, uint32_t count);
	uint32_t getBlockSize(uint32_t size);
} // namespace Memory

void* operator new(size_t size);
void* operator new[](size_t size);
void* operator new(size_t size, void* ptr);
void* operator new[](size_t size, void* ptr);
void  operator delete(void* ptr);
void  operator delete[](void* ptr);
void  operator delete(void* ptr, size_t size);
void  operator delete[](void* ptr, size_t size);