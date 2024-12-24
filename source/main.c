// Main entry point into PengOS. Inializes the kernel.

#include <gdt.c>
#include <io.h>
#include <isr.c>

// Entry point
void kernel_main(void)
{
	init_terminal();
	println("Welcome to %s PengOS!", "test");
	init_gdt();
	init_idt();
	init_isr();
}
