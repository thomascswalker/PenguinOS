/*
Main entry point into PengOS.
*/

#include <gdt.h>
#include <keyboard.h>
#include <list.h>
#include <memory.h>
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
	println("Welcome to PengOS!");

	uint32_t start = 0;
	uint32_t size = 0;
	Multiboot::init(info, &start, &size);
	Memory::init(start, size);

	while (1)
	{
	}
}