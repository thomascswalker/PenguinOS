/*
Main entry point into PenguinOS.
*/

#include <cmd.h>
#include <crt.h>
#include <filesystem.h>
#include <gdt.h>
#include <keyboard.h>
#include <memory.h>
#include <multiboot.h>
#include <pit.h>
#include <shell.h>

EXTERN void kmain(MultibootInfo* info, uint32_t magic)
{
	Shell::init();
	println("Initializing...");
	if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
	{
		panic("Invalid Multiboot magic value.");
	}
	uint32_t start = 0;
	uint32_t size = 0;
	Multiboot::init(info, &start, &size);

	GDT::init();
	IDT::init();
	PIT::init();
	Keyboard::init();

	// Once everything is initialized, enable interrupts.
	enableInterrupts();
	println("Welcome to Penguin OS!");

	Memory::init(start, size);

	// This needs to be called AFTER memory has been initialized.
	// Some constructors (like String, Array, etc.) which use
	// allocators need to use std::kmalloc.
	// TODO: Fix this!
	callConstructors();

	FileSystem::init();
	CMD::init();

	while (1)
	{
	}
}