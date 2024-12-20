/*
Main entry point into PengOS. Inializes the kernel.
*/

#include <gdt.h>
#include <io.h>

// Entry point
void kernel_main(void)
{
	gdt_install();
	init_terminal();
	println("Welcome to PengOS!");
	println("Made by Thomas Walker.");
}
