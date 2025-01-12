#include <memory.h>

static uint32_t* memory_map = 0;
static uint32_t	 max_blocks = 0;
static uint32_t	 used_blocks = 0;

static page_directory_t* current_page_directory = 0;
static paddr_t			 current_pd_address = 0;

// Sets a block/bit in the memory map, marking it as used.
void set_block(uint32_t bit)
{
	memory_map[bit / 32] |= (1 << (bit % 32));
}

// Unsets a block/bit in the memory map, marking it as free.
void unset_block(uint32_t bit)
{
	memory_map[bit / 32] &= ~(1 << (bit % 32));
}

// Tests if a block/bit in the memory map is used.
uint8_t test_block(uint32_t bit)
{
	return memory_map[bit / 32] & (1 << (bit % 32));
}

int32_t find_first_free_blocks(uint32_t block_count)
{
	if (!block_count)
	{
		panic("Block count is 0. Out of memory.");
		return ERR_MEMORY_UNAVAILABLE; // No memory
									   // available
	}

	// Test 32 blocks at a time
	uint32_t test_blocks = ceildiv(max_blocks, BLOCK_TEST_COUNT);
	debug("Testing %d blocks.", test_blocks);
	for (uint32_t i = 0; i < test_blocks; i++)
	{
		// Continue if all memory in this block is filled.
		if (memory_map[i] == 0xFFFFFFFF)
		{
			debug("Block %d is full; continuing...", i);
			continue;
		}

		// At least 1 bit is not set within this 32bit chunk of memory,
		//   find that bit by testing each bit
		for (int32_t j = 0; j < BLOCK_TEST_COUNT; j++)
		{
			// Get current bit to test
			int32_t bit = 1 << j;

			// If bit is set, found a used region of memory
			if (memory_map[i] & bit)
			{
				continue;
			}

			for (uint32_t count = 0, free_blocks = 0; count < block_count; count++)
			{
				if ((j + count > 31) && (i + 1 <= max_blocks / 32))
				{
					if (!(memory_map[i + 1] & (1 << ((j + count) - 32))))
					{
						free_blocks++;
					}
				}
				else
				{
					if (!(memory_map[i] & (1 << (j + count))))
					{
						free_blocks++;
					}
				}

				if (free_blocks == block_count)
				{
					debug("Found a free block at %x.", i * 32 + j);
					return i * 32 + j;
				}
			}
		}
	}

	panic("No free blocks found.");
	return ERR_MEMORY_UNAVAILABLE;
}

void init_pmm(void* start_address, uint32_t size)
{
	info("Initializing %dmb of physical memory (%x -> %x).", size / 0x400 / 0x400, start_address,
		start_address + size);
	memory_map = (uint32_t*)start_address;
	max_blocks = size / BLOCK_SIZE;
	used_blocks = size;

	memset(memory_map, 0xFF, max_blocks / BLOCKS_PER_BYTE);

	uint32_t  region_size = max_blocks / BLOCKS_PER_BYTE;
	uint32_t* base_address = memory_map;
	for (uint32_t i = 0; i < size; i++)
	{
		init_memory_region((uint32_t)base_address, region_size);
		base_address += region_size;
	}

	success("Physical memory initialized.");
}

void init_memory_region(uint32_t base_address, uint32_t size)
{
	int32_t align = base_address / BLOCK_SIZE; // Convert memory address to blocks
	int32_t block_count = size / BLOCK_SIZE;   // Convert size to blocks

	for (; block_count > 0; block_count--)
	{
		unset_block(align++);
		used_blocks--;
	}

	// Always going to set the 1st block, ensure we can't alloc 0, and can't overwrite
	//   interrupt tables IDT/IVT, Bios data areas, etc.
	set_block(0);
}

void deinit_memory_region(uint32_t base_address, uint32_t size)
{
	int32_t align = base_address / BLOCK_SIZE; // Convert memory address to blocks
	int32_t block_count = size / BLOCK_SIZE;   // Convert size to blocks

	for (; block_count > 0; block_count--)
	{
		set_block(align++);
		used_blocks++;
	}
}

uint32_t* allocate_blocks(uint32_t block_count)
{
	// If # of free blocks left is not enough, we can't allocate any more, return
	if ((max_blocks - used_blocks) <= block_count)
	{
		warning("Max=%d, Used=%d, Count=%d", max_blocks, used_blocks, block_count);
		panic("No free blocks available. Out of memory.");
		return 0;
	}

	int32_t starting_block = find_first_free_blocks(block_count);
	if (starting_block == -1)
	{
		warning("Could not find %d blocks in a row to allocate.", block_count);
		panic("Out of memory.");
		return 0; // Couldn't find that many blocks in a row to allocate
	}

	// Found free blocks, set them as used
	for (uint32_t i = 0; i < block_count; i++)
	{
		set_block(starting_block + i);
	}

	used_blocks += block_count; // Blocks are now used/reserved, increase count

	// Convert blocks to bytes to get start of actual RAM that is now allocated
	uint32_t address = starting_block * BLOCK_SIZE;
	if (!address)
	{
		panic("Out of memory.");
	}

	return (uint32_t*)address; // Physical memory location of allocated blocks
}

void free_blocks(uint32_t* address, uint32_t block_count)
{
	int32_t starting_block = (uint32_t)address / BLOCK_SIZE; // Convert address to blocks

	for (uint32_t i = 0; i < block_count; i++)
	{
		unset_block(starting_block + i); // Unset bits/blocks in memory map, to free
	}

	used_blocks -= block_count; // Decrease used block count}
}

// Get entry in page table for the given address.
pt_entry_t* get_pt_entry(page_table_t* t, const vaddr_t address)
{
	if (t)
	{
		return &t->entries[PT_INDEX(address)];
	}
	return nullptr;
}

// Get entry in page directory for the given address.
pd_entry_t* get_pd_entry(page_directory_t* d, const vaddr_t address)
{
	if (d)
	{
		return &d->entries[PT_INDEX(address)];
	}
	return nullptr;
}

// Return a page for a given virtual address in the current
// page directory.
pt_entry_t* get_page(const vaddr_t address)
{
	// Get page directory
	page_directory_t* pd = current_page_directory;

	// Get page table in directory
	pd_entry_t*	  entry = &pd->entries[PD_INDEX(address)];
	page_table_t* table = (page_table_t*)PAGE_PHYS_ADDRESS(entry);

	// Get page in table
	pt_entry_t* page = &table->entries[PT_INDEX(address)];

	// Return page
	return page;
}

// Allocate a page of memory
void* allocate_page(pt_entry_t* page)
{
	void* block = allocate_blocks(1);
	if (block)
	{
		// Map page to block. Ask for a new physical address of memory
		// for this virtual address.
		SET_FRAME(page, (paddr_t)block);
		SET_ATTRIBUTE(page, PTE_PRESENT);
	}
	return block;
}

// Free a page of memory.
void free_page(pt_entry_t* page)
{
	void* address = (void*)PAGE_PHYS_ADDRESS(page);
	if (address)
	{
		free_blocks(address, 1);
	}
	CLEAR_ATTRIBUTE(page, PTE_PRESENT);
}

// Set the current page directory to the specified page directory.
bool set_page_directory(page_directory_t* pd)
{
	if (!pd)
	{
		return false;
	}

	current_page_directory = pd;

	// CR3 holds the address of the current page directory
	asm("mov %%cr3, %%eax" : : "a"(current_page_directory));

	return true;
}

// Invalidate the specified virtual address.
void flush_tlb_entry(vaddr_t address)
{
	asm("cli\ninvlpg (%0)\nsti" : : "r"(address));
}

bool map_page(void* paddr, void* vaddr)
{
	debug("Mapping page from PHYS%x -> VIRT%x.", (uint32_t*)paddr, (uint32_t*)vaddr);
	// Get page entry
	page_directory_t* pd = current_page_directory;
	pd_entry_t*		  entry = &pd->entries[PD_INDEX((uint32_t)vaddr)];

	if ((*entry & PTE_PRESENT) != PTE_PRESENT)
	{
		// Page table not present, allocate it.
		page_table_t* table = (page_table_t*)allocate_blocks(1);
		if (!table)
		{
			return false; // Out of memory
		}

		memset(table, 0, sizeof(page_table_t));

		// cReate new entry
		pd_entry_t* entry = &pd->entries[PD_INDEX((uint32_t)vaddr)];

		// Map in table and enable attributes
		SET_ATTRIBUTE(entry, PDE_PRESENT);
		SET_ATTRIBUTE(entry, PDE_READ_WRITE);
		SET_FRAME(entry, (paddr_t)table);
	}

	// Get table
	page_table_t* table = (page_table_t*)PAGE_PHYS_ADDRESS(entry);

	// Get page in table
	pt_entry_t* page = &table->entries[PT_INDEX((uint32_t)vaddr)];

	// Map in page
	SET_FRAME(page, (uint32_t)paddr);
	SET_ATTRIBUTE(page, (uint32_t)paddr);

	return true;
}

bool unmap_page(void* vaddr)
{
	pt_entry_t* page = get_page((uint32_t)vaddr);
	SET_FRAME(page, 0);					// Set physical address to 0
	CLEAR_ATTRIBUTE(page, PTE_PRESENT); // Set as not present, will trigger a page fault exception
	return true;
}

bool init_vmm()
{
	info("Initializing virtual memory...");
	page_directory_t* dir = (page_directory_t*)allocate_blocks(1);

	if (!dir)
	{
		panic("Unable to allocate block for page directory. Out of memory.");
		return false; // Out of memory
	}

	// Clear current page directory
	uint32_t pd_size = sizeof(page_directory_t);
	memset(dir, 0, pd_size);
	for (uint32_t i = 0; i < 1024; i++)
	{
		pt_entry_t* entry = &dir->entries[i];
		CLEAR_ATTRIBUTE(entry, PTE_PRESENT);
		SET_ATTRIBUTE(entry, PTE_READ_WRITE);
	}

	// Allocate a default page table
	page_table_t* table_low = (page_table_t*)allocate_blocks(1);
	if (!table_low)
	{
		panic("Out of memory.");
		return false; // Out of memory
	}

	// Allocate a 3GB page table
	page_table_t* table_high = (page_table_t*)allocate_blocks(1);
	if (!table_high)
	{
		panic("Out of memory.");
		return false; // Out of memory
	}

	// Clear each table we just created
	memset(table_low, 0, sizeof(page_table_t));
	memset(table_high, 0, sizeof(page_table_t));

	// Identity map the first 4MB of memory
	{						  // Closed scope
		uint32_t frame = 0x0; // Current frame
		uint32_t vaddr = 0x0; // Current virtual address
		for (uint32_t i = 0; i < 1024; i++)
		{
			// Create a new page
			pt_entry_t page = 0;
			SET_ATTRIBUTE(&page, PTE_PRESENT);
			SET_ATTRIBUTE(&page, PTE_READ_WRITE);
			SET_FRAME(&page, frame);

			// Add page to higher table
			table_high->entries[PT_INDEX(vaddr)] = page;

			// Increment the frame and virtual address
			frame += PAGE_SIZE;
			vaddr += PAGE_SIZE;
		}
	}

	// Map kernel to 3GB+ addresses (higher-half kernel)
	{									// Closed scope
		uint32_t frame = 0x000FFFFF;	// Start at 1MB
		uint32_t vaddr = VIRTUAL_START; // Start at the kernel offset (3GB)
		for (uint32_t i = 0; i < 1024; i++)
		{
			// Create a new page
			pt_entry_t page = 0;
			SET_ATTRIBUTE(&page, PTE_PRESENT);
			SET_ATTRIBUTE(&page, PTE_READ_WRITE);
			SET_FRAME(&page, frame);

			// Add page to higher table
			table_low->entries[PT_INDEX(vaddr)] = page;

			// Increment the frame and virtual address
			frame += PAGE_SIZE;
			vaddr += PAGE_SIZE;
		}
	}

	pd_entry_t* entry_high = &dir->entries[PD_INDEX(VIRTUAL_START)];
	SET_ATTRIBUTE(entry_high, PDE_PRESENT);
	SET_ATTRIBUTE(entry_high, PDE_READ_WRITE);
	SET_FRAME(entry_high, (paddr_t)table_low); // High entry points to default (low) page table.

	pd_entry_t* entry_low = &dir->entries[PD_INDEX(0x00000000)];
	SET_ATTRIBUTE(entry_low, PDE_PRESENT);
	SET_ATTRIBUTE(entry_low, PDE_READ_WRITE);
	SET_FRAME(entry_low, (paddr_t)table_high); // Low entry points to 3GB (high) page table.

	// Switch to page directory
	if (!set_page_directory(dir))
	{
		error("Failed to set current page directory.");
		return false;
	}

	success("Virtual memory intialized.");

	return true;
}
