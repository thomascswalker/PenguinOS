// Main entry point into PengOS. Inializes the kernel.

#include <gdt.c>
#include <timer.c>

// Entry point
void kernel_main(void)
{
	init_terminal();
	info("Welcome to PengOS!");
	disable_interrupts();
	init_gdt();
	init_idt();
	init_timer();
	enable_interrupts();

	while (true)
	{
	}
}