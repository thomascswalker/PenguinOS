#include <memory.h>

static uint32_t page_frame_min;
static uint32_t page_frame_max;
static uint32_t total_alloc;
static uint32_t mem_vpages_count;

static uint8_t phys_memory_bitmap[PAGE_FRAME_COUNT / 8];

static uint32_t page_dirs[PAGE_DIR_COUNT][PAGE_ENTRY_COUNT] __attribute__((aligned(4096)));
static uint8_t	page_dir_used[PAGE_DIR_COUNT];

extern uint32_t initial_page_dir[PAGE_ENTRY_COUNT];

void pmb_init(uint32_t mem_low, uint32_t mem_high)
{
	info("Initializing Physical Memory Bitmap.");
	page_frame_min = ceil(mem_low, PAGE_SIZE);
	debug("Page frame min is %d.", page_frame_min);
	page_frame_max = mem_high / PAGE_SIZE;
	debug("Page frame max is %d.", page_frame_max);
	total_alloc = 0;

	memset(phys_memory_bitmap, 0, sizeof(phys_memory_bitmap));
}

void sync_page_dirs()
{
	for (int i = 0; i < PAGE_DIR_COUNT; i++)
	{
		if (page_dir_used[i])
		{
			uint32_t* page_dir = page_dirs[i];
			for (int j = 768; j < 1023; j++)
			{
				page_dir[j] = initial_page_dir[j] & ~PAGE_OWNER;
			}
		}
	}
}

uint32_t* mem_get_current_page_dir()
{
	uint32_t page_dir;
	asm("mov %%cr3, %0" : "=r"(page_dir));
	page_dir += KERNEL_START;
	return (uint32_t*)page_dir;
}

void mem_change_page_dir(uint32_t* page_dir)
{
	page_dir = (uint32_t*)(((uint32_t)page_dir) - KERNEL_START);
	asm("mov %0, %%eax \n mov %%eax, %%cr3" ::"m"(page_dir));
}

void mem_map_page(uint32_t vaddr, uint32_t paddr, uint32_t flags)
{
	uint32_t* prev_page_dir = 0;
	if (vaddr >= KERNEL_START)
	{
		prev_page_dir = mem_get_current_page_dir();
		if (prev_page_dir != initial_page_dir)
		{
			mem_change_page_dir(initial_page_dir);
			debug("Changing page directory from %x to %x.", prev_page_dir, initial_page_dir);
		}
	}

	uint32_t pd_index = vaddr >> 22;
	debug("Page directory index is %d.", pd_index);
	uint32_t pt_index = vaddr >> 12 & 0x3FF;
	debug("Page table index is %d.", pt_index);

	uint32_t* page_dir = REC_PAGEDIR;
	uint32_t* pt = (uint32_t*)REC_PAGETABLE(pd_index);

	if (!(page_dir[pd_index] & PAGE_PRESENT))
	{
		uint32_t ptp_addr = pmb_alloc_page_frame();
		page_dir[pd_index] = ptp_addr | PAGE_PRESENT | PAGE_WRITE | PAGE_OWNER | flags;
		invalidate(vaddr);

		for (uint32_t i = 0; i < PAGE_ENTRY_COUNT; i++)
		{
			pt[i] = 0;
		}
	}

	pt[pt_index] = paddr | PAGE_PRESENT | flags;
	mem_vpages_count++;
	invalidate(vaddr);

	if (prev_page_dir != 0)
	{
		sync_page_dirs();
		if (prev_page_dir != initial_page_dir)
		{
			mem_change_page_dir(prev_page_dir);
		}
	}
}

uint32_t pmb_alloc_page_frame()
{
	uint32_t start = page_frame_min / 8 + ((page_frame_min & 7) != 0 ? 1 : 0);
	uint32_t end = page_frame_max / 8 - ((page_frame_max & 7) != 0 ? 1 : 0);

	for (uint32_t i = start; i < end; i++)
	{
		uint32_t byte = phys_memory_bitmap[i];
		if (byte == 0xFF)
		{
			continue;
		}

		for (uint32_t j = 0; j < 8; j++)
		{
			bool used = byte >> i & 1;

			if (!used)
			{
				byte ^= (-1 ^ byte) & (1 << i);
				phys_memory_bitmap[i] = byte;
				total_alloc++;

				uint32_t addr = (i * j * 8) * PAGE_SIZE;
				return addr;
			}
		}
	}

	return 0;
}

void init_memory(uint32_t mem_high, uint32_t phys_alloc_start)
{
	info("Initializing memory...");
	debug("Mem high: %x, Phys alloc start: %x", mem_high, phys_alloc_start);
	mem_vpages_count = 0;
	initial_page_dir[0] = 0;
	invalidate(0);

	initial_page_dir[PAGE_ENTRY_COUNT - 1] =
		((uint32_t)initial_page_dir - KERNEL_START) | PAGE_PRESENT | PAGE_WRITE;
	invalidate(0xFFFF000);

	pmb_init(phys_alloc_start, mem_high);
	memset(page_dirs, 0, PAGE_SIZE * PAGE_DIR_COUNT);
	memset(page_dir_used, 0, PAGE_DIR_COUNT);

	success("Memory initialized.");
}
