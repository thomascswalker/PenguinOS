// Main entry point into PengOS. Inializes the kernel.

#include <gdt.c>
#include <idt.c>
#include <io.h>

// Entry point
void kernel_main(void)
{
	init_terminal();
	println("Welcome to PengOS!");
	init_gdt();
	init_idt();
}
