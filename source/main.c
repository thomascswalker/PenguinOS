/*
Main entry point into PengOS. Initializes the kernel.

Kernel is 8MB in total.
- The first 1MB is reserved.
- The following 3MB (1..4) are for the heap.
- The remaining 4MB (5..8) are for paging.

---------------------------------
| 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | MB
---------------------------------
|   |   Heap    |    Paging     |
---------------------------------
*/

#include <cpuid.h>
#include <gdt.h>
#include <keyboard.h>
#include <memory.h>
#include <timer.h>

void kernel_main(multiboot_info_t* boot_info)
{

	init_terminal();
	println("Welcome to PengOS!");

	uint32_t mod1 = *(uint32_t*)(boot_info->mods_addr + 4);
	uint32_t phys_alloc_start = (mod1 + 0xFFF) & ~0xFFF;
	init_memory(boot_info->mem_upper * 1024, phys_alloc_start);
	success("Memory allocation complete.");

	init_gdt();
	init_idt();
	init_timer();
	init_keyboard();

	enable_interrupts();
	while (true)
	{
	}
}