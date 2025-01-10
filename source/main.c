/*
Main entry point into PengOS. Initializes the kernel.
*/

#include <cpuid.h>
#include <gdt.h>
#include <keyboard.h>
#include <memory.h>
#include <multiboot.h>
#include <timer.h>

void dump_memory_map(multiboot_mmap_t* mmap)
{
	for (uint32_t i = 0; i < mmap->count; i++)
	{
		multiboot_mmap_entry_t* mmmt = &mmap->entries[i];
		const char*				fmt = "Start addr: %x | Length: %x | Size: %x | Type: %d";
		debug(fmt, mmmt->addr_low, mmmt->len_low, mmmt->size, mmmt->type);
	}
}

void kernel_main(multiboot_info_t* boot_info)
{

	init_terminal();
	println("Welcome to PengOS!");

	init_gdt();
	init_idt();
	init_timer();
	init_keyboard();
	enable_interrupts();

	multiboot_mmap_t mmap;
	mmap.count = boot_info->mmap_length / sizeof(multiboot_mmap_entry_t);
	debug("MMAP count is %d.", mmap.count);
	debug("Size of mmap_entry_t is %d.", sizeof(multiboot_mmap_entry_t));
	multiboot_mmap_entry_t* entry = (multiboot_mmap_entry_t*)boot_info->mmap_addr;
	for (uint32_t i = 0; i < mmap.count; i++)
	{
		mmap.entries[i] = *entry; // Store entry in array
	}
	// for (uint32_t i = 0; i < mmap.count; i += sizeof(multiboot_mmap_entry_t))
	// {
	// 	multiboot_mmap_entry_t* entry = (multiboot_mmap_entry_t*)&mmap.entries[i];
	// 	entry = (multiboot_mmap_entry_t*)(boot_info->mmap_addr + i);
	// }
	dump_memory_map(&mmap);

	uint32_t phys_alloc_start = boot_info->mmap_addr;
	debug("Physical address start is %x.", phys_alloc_start);
	uint32_t total_memory = boot_info->mem_lower + boot_info->mem_upper;
	debug("Total memory is %dKB.", total_memory);
	init_pmm(phys_alloc_start, total_memory);
	init_vmm();

	while (true)
	{
	}
}