// Main entry point into PengOS. Inializes the kernel.

#include <gdt.h>
#include <io.h>

// Entry point
void kernel_main(void)
{
	init_terminal();
	println("Welcome to PengOS!");
	init_gdt();
}
