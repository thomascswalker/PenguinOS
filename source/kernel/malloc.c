#include <malloc.h>
#include <memory.h>

static uint32_t heap_start;
static uint32_t heap_size;
static uint32_t threshold;
static bool		kmalloc_initialized = false;

void init_kmalloc(uint32_t initial_heap_size)
{
	info("Initializing heap at %x with size %d.", HEAP_START, initial_heap_size);
	heap_start = HEAP_START;
	heap_size = 0;
	threshold = 0;
	kmalloc_initialized = true;

	change_heap_size(initial_heap_size);
	success("Heap initialized.");
}

void change_heap_size(uint32_t new_size)
{
	uint32_t old_page_top = ceil(heap_size, PAGE_SIZE);
	debug("Old page top: %d", old_page_top);
	uint32_t new_page_top = ceil(new_size, PAGE_SIZE);
	debug("New page top: %d", new_page_top);

	uint32_t diff = new_page_top - old_page_top;
	debug("Page top diff: %d", diff);
	for (uint32_t i = 0; i < diff; i++)
	{
		uint32_t paddr = alloc_page_frame();
		uint32_t vaddr = HEAP_START + (old_page_top * PAGE_SIZE) + (i * PAGE_SIZE);
		debug("Mapping page %d from PHYS:%x to VIRT:%x.", i, paddr, vaddr);
		map_page(vaddr, paddr, PAGE_WRITE);
	}
}
