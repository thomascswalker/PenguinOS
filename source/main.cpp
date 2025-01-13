/*
Main entry point into PengOS. Initializes the kernel.
*/

#include <gdt.h>
#include <keyboard.h>
#include <pit.h>

EXTERN void kmain() /*multiboot_info_t* boot_info*/
{
	Terminal::init();
	println("Welcome to PengOS!");

	GDT::init();
	IDT::init();
	PIT::init();
	Keyboard::init();

	// Once everything is initialized, enable interrupts.
	enableInterrupts();

		// uint32_t paddr = boot_info->mmap_addr;
	// debug("Physical address start is %x.", paddr);
	// uint32_t psize = boot_info->mem_lower + boot_info->mem_upper;
	// debug("Total memory is %dKB.", psize);
	// init_pmm(paddr, psize);
	// init_vmm();

	while (1)
	{
	}
}