/*
Main entry point into PengOS. Initializes the kernel.
*/

#include <gdt.h>
#include <keyboard.h>
#include <multiboot.h>
#include <paging.h>
#include <pit.h>

static uint32_t heapSize = 0;
static uint32_t heapAddr = 0;

EXTERN void kmain(MultibootInfo* info, uint32_t magic)
{
	VGA::init();
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
	debug("Physical Memory: %x => %x (%dMB)", address, address + size, (size / 1024 / 1024));
	Paging::init();

	println("Welcome to PengOS!");

	while (1)
	{
	}
}