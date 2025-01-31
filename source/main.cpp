/*
Main entry point into PengOS. Initializes the kernel.
*/

#include <gdt.h>
#include <keyboard.h>
#include <multiboot.h>
#include <pit.h>

EXTERN void kmain(MultibootInfo* info, uint32_t magic)
{
	VGA::init();
	println("Initializing PengOS...");
	if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
	{
		panic("Invalid Multiboot magic value.");
	}
	GDT::init();
	IDT::init();
	PIT::init();
	Keyboard::init();

	// Once everything is initialized, enable interrupts.
	enableInterrupts();

	uint32_t address = 0;
	uint32_t size = 0;
	Multiboot::init(info, &address, &size);

	println("Welcome to PengOS!");

	while (1)
	{
	}
}