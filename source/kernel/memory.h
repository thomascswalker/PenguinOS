#pragma once

#include <math.h>
#include <multiboot.h>
#include <stdio.h>

#define KERNEL_START 0xC0000000
#define HEAP_START 0xD0000000

#define REC_PAGEDIR ((uint32_t*)0xFFFFF000)
#define REC_PAGETABLE(i) (0xFFC00000 + (i << 12))

#define PAGE_SIZE 0x1000
#define PAGE_ENTRY_COUNT 1024
#define PAGE_DIR_COUNT 256
#define PAGE_FRAME_COUNT (0x10000000 / PAGE_SIZE / 8)

enum page_flag
{
	PAGE_PRESENT = (1 << 0),
	PAGE_WRITE = (1 << 1),
	PAGE_OWNER = (1 << 9),
};
typedef enum page_flag page_flag_t;

void	  init_memory(uint32_t mem_high, uint32_t phys_alloc_start);
void	  init_pmb(uint32_t mem_low, uint32_t mem_high);
uint32_t  alloc_page_frame();
uint32_t* get_current_page_dir();
void	  change_page_dir(uint32_t* page_dir);
void	  map_page(uint32_t vaddr, uint32_t paddr, uint32_t flags);
void	  sync_page_dirs();
