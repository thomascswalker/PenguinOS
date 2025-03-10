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
#include <scheduling.h>
#include <shell.h>

void fproc0()
{
	while (1)
	{
		debug("1.");
		Scheduling::schedule();
	}
}
void fproc1()
{
	while (1)
	{
		debug("2.");
		Scheduling::schedule();
	}
}

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
	FileSystem::init();
	CMD::init();

	println("Welcome to Penguin OS!");

	// Task proc0;
	// Task proc1;

	// Scheduling::createTask(&fproc0);
	// Scheduling::createTask(&fproc1);

	while (1)
	{
	}
}