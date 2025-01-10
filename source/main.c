/*
Main entry point into PengOS. Initializes the kernel.
*/

#include <cpuid.h>
#include <gdt.h>
#include <keyboard.h>
#include <memory.h>
#include <multiboot.h>
#include <syscall.h>
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

	uint32_t paddr = boot_info->mmap_addr;
	debug("Physical address start is %x.", paddr);
	uint32_t psize = boot_info->mem_lower + boot_info->mem_upper;
	debug("Total memory is %dKB.", psize);
	init_pmm(paddr, psize);
	init_vmm();
	enable_interrupts();

	sleep(5);

	while (true)
	{
	}
}