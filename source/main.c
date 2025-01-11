/*
Main entry point into PengOS. Initializes the kernel.
*/

#include <cpuid.h>
#include <gdt.h>
#include <keyboard.h>
#include <malloc.h>
#include <memory.h>
#include <multiboot.h>
#include <syscall.h>
#include <timer.h>

extern uint32_t kernel_start;
extern uint32_t kernel_end;

void kernel_main(multiboot_info_t* boot_info)
{
	init_terminal();
	println("Welcome to PengOS!");
	init_gdt();
	init_idt();
	init_timer();
	init_keyboard();

	// Get the starting address for physical memory
	uint32_t paddr = boot_info->mmap_addr;
	// Get the size for physical memory
	uint32_t psize = boot_info->mem_lower + boot_info->mem_upper;
	// Initialize physical memory.
	init_pmm(paddr, psize);
	// Initialize virtual memory.
	init_vmm();

	// After memory is intialized, enable interrupts (page fault, etc.)
	enable_interrupts();

	// Test malloc
	void* ptr = malloc(10000);
	debug("ptr: %x", (uint32_t*)ptr);
	free(ptr);

	while (true)
	{
	}
}