#pragma once

#include <math.h>
#include <multiboot.h>
#include <stdio.h>

#define KERNEL_START 0xC0000000
#define HEAP_START 0xD0000000

#define BLOCK_SIZE 0x1000
#define BLOCKS_PER_BYTE 8
#define BLOCK_TEST_COUNT 32

#define PAGE_SIZE 0x1000
#define PAGES_PER_TABLE 0x0400
#define TABLES_PER_DIRECTORY 0x0400

#define ERR_MEMORY_UNAVAILABLE -1

typedef uint32_t pt_entry_t; // Page table entry
typedef uint32_t pd_entry_t; // Page directory entry
typedef uint32_t paddr_t;	 // Physical address
typedef uint32_t vaddr_t;	 // Virtual address

// The first 22 bits of the virtual address specify the index of the page directory.
#define PD_INDEX(addr) ((addr) >> 22)
// The first 12 bits of the virtual address specify the index of the page table.
// Max index within a page table is 1023, so we & with 0x3FF.
#define PT_INDEX(addr) (((addr) >> 12) & 0x3FF)
// Clear lowest 12 bits, only return frame/address.
#define PAGE_PHYS_ADDRESS(dir_entry) ((*dir_entry) & ~0xFFFF)
#define SET_ATTRIBUTE(entry, attr) (*entry |= attr)
#define CLEAR_ATTRIBUTE(entry, attr) (*entry &= ~attr)
#define TEST_ATTRIBUTE(entry, attr) (*entry & attr)
// Only set address frame, not flags
#define SET_FRAME(entry, addr) (*entry = (*entry & ~0x7FFFF000) | addr)

// Page table entry
enum pte
{
	PTE_PRESENT = 0x01,
	PTE_READ_WRITE = 0x02,
	PTE_USER = 0x04,
	PTE_WRITE_THROUGH = 0x08,
	PTE_CACHE_DISABLE = 0x10,
	PTE_ACCESSED = 0x20,
	PTE_DIRTY = 0x40,
	PTE_PAT = 0x80, // Page Attribute Table
	PTE_GLOBAL = 0x100,
	PTE_FRAME = 0x7FFFF000 // Bits 12+
};
// Page table entry
typedef enum pte pte_t;

// Page directory entry
enum pde
{
	PDE_PRESENT = 0x01,
	PDE_READ_WRITE = 0x02,
	PDE_USER = 0x04,
	PDE_WRITE_THROUGH = 0x08,
	PDE_CACHE_DISABLE = 0x10,
	PDE_ACCESSED = 0x20,
	PDE_DIRTY = 0x40,	   // 4MB entries only
	PDE_PAGE_SIZE = 0x80,  // 0 = 4KB PAGE, 1 = 4MB page
	PDE_GLOBAL = 0x100,	   // 4MB entries only
	PDE_PAT = 0x2000,	   // 4MB entries only
	PDE_FRAME = 0x7FFFF000 // Bits 12+
};
// Page directory entry
typedef enum pde pde_t;

// Page table: handle 4MB each, 1024 entries * 4096
typedef struct page_directory
{
	pt_entry_t entries[PAGES_PER_TABLE];
} page_directory_t;

// Page directory: handle 4GB each, 1024 tables * 4MB
typedef struct page_table
{
	pd_entry_t entries[TABLES_PER_DIRECTORY];
} page_table_t;

// Physical memory

void	  set_block(uint32_t bit);
void	  unset_block(uint32_t bit);
uint8_t	  test_block(uint32_t bit);
int32_t	  find_first_free_blocks(uint32_t block_count);
void	  init_pmm(uint32_t start_address, uint32_t size);
void	  init_memory_region(uint32_t base_address, uint32_t size);
void	  deinit_memory_region(uint32_t base_address, uint32_t size);
uint32_t* allocate_blocks(uint32_t block_count);
void	  free_blocks(uint32_t* address, uint32_t block_count);

// Virtual memory

pt_entry_t* get_pt_entry(page_table_t* t, const vaddr_t address);
pd_entry_t* get_pd_entry(page_directory_t* d, const vaddr_t address);
pt_entry_t* get_page(const vaddr_t address);
void*		allocate_page(pt_entry_t* page);
void		free_page(pt_entry_t* page);
bool		set_page_directory(page_directory_t* pd);
void		flush_tlb_entry(vaddr_t address);
bool		map_page(void* paddr, void* vaddr);
bool		unmap_page(void* vaddr);
bool		init_vmm();
EXTERN void enable_paging(uint32_t* address); // Defined in boot.s