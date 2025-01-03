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
#include <gdt.c>
#include <keyboard.c>
// #include <paging.c>
#include <timer.c>

void kernel_main(void)
{

	init_terminal();
	info("Welcome to PengOS! %s");
	// print_cpuid();
	disable_interrupts();
	init_keyboard();
	init_gdt();
	init_idt();
	init_timer();

	while (true)
	{
	}
}