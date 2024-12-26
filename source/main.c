// Main entry point into PengOS. Inializes the kernel.

#include <gdt.c>
#include <isr.c>

// Entry point
void kernel_main(void)
{
	init_terminal();
	println("Welcome to PengOS!");
	init_gdt();
	remap_pic();
	init_idt();

	__asm__("div %0" ::"r"(0));
}
