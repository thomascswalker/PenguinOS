#pragma once

#include <stddef.h>
#include <stdint.h>

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

#define MIN_ORDER 5								 // The smallest block is 32 bytes = 2^5.
#define MAX_ORDER 12							 // The largest block is 4096 bytes = 2^12.
#define BUCKET_COUNT (MAX_ORDER - MIN_ORDER + 1) // 8 buckets in total
#define POOL_SIZE ((1 << MAX_ORDER) * 1024)		 // Memory pool is 4K * 1K = 4MB

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
	uint8_t		  order;  // The bucket index (0 for 32 bytes, 7 for 4096 bytes)
	uint8_t		  isFree; // 1 if the block is free, 0 if allocated
	struct Block* next;	  // Pointer for linking free blocks in a list
};

namespace Memory
{
	void init(uint32_t start, uint32_t size);

	/* Paging */

	void identityMapTable(uint32_t index);

	uint32_t* getTableFromAddress(uint32_t address);
	uint32_t* getTableFromIndex(uint32_t index);

	void enablePaging();
	void setPageDirectory(const uint32_t* directory);
	void setLargePaging(bool state);

	void dumpPageTable();

	/* Memory Allocation */
	void addBlock(Block* block);
	void removeBlock(Block* block, uint32_t order);

	void* kmalloc(size_t size);
	void  kfree(void* ptr);
	void* krealloc(void* ptr, size_t newSize);
} // namespace Memory

void* operator new(size_t size);
void* operator new[](size_t size);
void* operator new(size_t size, void* ptr);
void* operator new[](size_t size, void* ptr);
void  operator delete(void* ptr);
void  operator delete[](void* ptr);
void  operator delete(void* ptr, size_t size);
void  operator delete[](void* ptr, size_t size);