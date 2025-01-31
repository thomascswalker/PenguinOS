/*
Main entry point into PengOS. Initializes the kernel.
*/

#include <gdt.h>
#include <keyboard.h>
#include <memory.h>
#include <multiboot.h>
#include <paging.h>
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
	Memory::init(address, size);
	Paging::init();

	println("Welcome to PengOS!");

	void* p0 = kmalloc(5000);
	void* p1 = kmalloc(512);
	void* p2 = kmalloc(49832);
	void* p3 = kmalloc(32);
	debugx(p0);
	debugx(p1);
	debugx(p2);
	debugx(p3);

	while (1)
	{
	}
}