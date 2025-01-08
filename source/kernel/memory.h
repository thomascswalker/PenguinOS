#include <math.h>
#include <multiboot.h>
#include <stdio.h>

#define KERNEL_START 0xC0000000
#define PAGE_FLAG_PRESENT (1 << 0)
#define PAGE_FLAG_WRITE (1 << 1)
static uint32_t page_frame_min;
static uint32_t page_frame_max;
static uint32_t total_alloc;

// TODO: Refactor as bitarray
#define PAGE_DIR_COUNT 256
#define PAGE_FRAME_COUNT (0x10000000 / 0x1000 / 8)
static uint8_t phys_memory_bitmap[PAGE_FRAME_COUNT / 8];

static uint32_t page_dirs[PAGE_DIR_COUNT][1024] __attribute__((aligned(4096)));
static uint8_t	page_dir_used[PAGE_DIR_COUNT];

extern uint32_t initial_page_dir[1024];

void invalidate(uint32_t vaddr)
{
	asm("invlpg %0" ::"m"(vaddr));
}

void pmm_init(uint32_t mem_low, uint32_t mem_high)
{
	page_frame_min = ceil(mem_low, 0x1000);
	page_frame_max = mem_high / 0x1000;
	total_alloc = 0;

	memset(phys_memory_bitmap, 0, sizeof(phys_memory_bitmap));
}

void init_memory(uint32_t mem_high, uint32_t phys_alloc_start)
{
	initial_page_dir[0] = 0;
	invalidate(0);

	initial_page_dir[1023] =
		((uint32_t)initial_page_dir - KERNEL_START) | PAGE_FLAG_PRESENT | PAGE_FLAG_WRITE;
	invalidate(0xFFFF000);

	pmm_init(phys_alloc_start, mem_high);
	memset(page_dirs, 0, 0x1000 * PAGE_DIR_COUNT);
	memset(page_dir_used, 0, PAGE_DIR_COUNT);
}