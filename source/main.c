// Main entry point into PengOS. Inializes the kernel.

#include <cpuid.h>
#include <gdt.c>
#include <keyboard.c>
#include <timer.c>

// Entry point
void kernel_main(void)
{
	init_terminal();
	info("Welcome to PengOS! %s");
	print_cpuid();
	disable_interrupts();
	init_keyboard();
	init_gdt();
	init_idt();
	init_timer();
	while (true)
	{
	}
}