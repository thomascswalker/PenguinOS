/*
Main entry point into PenguinOS.
*/

#include <cmd.h>
#include <crt.h>
#include <elf.h>
#include <filesystem.h>
#include <gdt.h>
#include <keyboard.h>
#include <list.h>
#include <memory.h>
#include <multiboot.h>
#include <pit.h>
#include <scheduler.h>
#include <shell.h>

EXTERN void kmain(MultibootInfo* info, uint32_t magic)
{
	Shell::init();
	if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
	{
		panic("Invalid Multiboot magic value.");
	}
	uint32_t start = 0;
	uint32_t size = 0;
	Multiboot::init(info, &start, &size);
	printf("RAM: %dMB\n", (size / 1024 / 1024));

	GDT::init();
	IDT::init();
	PIT::init();
	Keyboard::init();

	// Once everything is initialized, enable interrupts.
	enableInterrupts();

	Memory::init(start, size);
	Scheduler::init();
	IDE::init();
	CMD::init();

	println("Welcome to Penguin OS!");

	FAT32FileSystem fs;
	setVirtualFileSystem(&fs);

	const char* filename = "/";
	auto		files = fs.getFilesInDirectoryFromName("/etc");
	for (const auto& file : files)
	{
		printf("File: %s\n", file->name);
	}

	while (1)
	{
	}
}