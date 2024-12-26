// Main entry point into PengOS. Inializes the kernel.

#include <gdt.c>
#include <timer.c>

// Entry point
void kernel_main(void)
{
	init_terminal();
	println("Welcome to PengOS!");
	init_gdt();
	remap_pic();
	init_idt();
	init_timer(100);

	// __asm__("div %0" ::"r"(0));
}
