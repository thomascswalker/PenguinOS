#pragma once

#include <math.h>
#include <multiboot.h>
#include <stdio.h>

#define KERNEL_START 0xC0000000
#define HEAP_START 0xD0000000

#define PAGE_SIZE 0x1000
#define PAGES_PER_TABLE 0x0400
#define TABLES_PER_DIRECTORY 0x0400

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
enum ptf
{
	PTE_PRESENT = 0x01,
	PTE_WRITE = 0x02,
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
typedef enum ptf ptf_t;

// Page directory entry
enum pdf
{
	PDF_PRESENT = 0x01,
	PDF_WRITE = 0x02,
	PDF_USER = 0x04,
	PDF_WRITE_THROUGH = 0x08,
	PDF_CACHE_DISABLE = 0x10,
	PDF_ACCESSED = 0x20,
	PDF_DIRTY = 0x40,	   // 4MB entries only
	PDF_PAGE_SIZE = 0x80,  // 0 = 4KB PAGE, 1 = 4MB page
	PDF_GLOBAL = 0x100,	   // 4MB entries only
	PDF_PAT = 0x2000,	   // 4MB entries only
	PDF_FRAME = 0x7FFFF000 // Bits 12+
};
// Page directory entry
typedef enum pdf pdf_t;

// Page table: handle 4MB each, 1024 entries * 4096
typedef struct
{
	pt_entry_t entries[PAGES_PER_TABLE];
} page_directory_t;

// Page directory: handle 4GB each, 1024 tables * 4MB
typedef struct
{
	pd_entry_t entries[TABLES_PER_DIRECTORY];
} page_table_t;

page_directory_t* current_page_directory = 0;
paddr_t			  current_page_directory_address = 0;