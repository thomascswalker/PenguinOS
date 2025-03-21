/*
Main entry point into PenguinOS.
*/

#include <cmd.h>
#include <crt.h>
#include <filesystem.h>
#include <gdt.h>
#include <keyboard.h>
#include <list.h>
#include <memory.h>
#include <multiboot.h>
#include <pit.h>
#include <scheduler.h>
#include <shell.h>

static uint32_t i = 0;
static uint32_t j = 0;

void process1()
{
	while (1)
	{
		printf("Process 1 is running: %d\n", i++);
		System::schedule();
	}
}

void process2()
{
	while (1)
	{
		printf("Process 2 is running: %d\n", j++);
		System::schedule();
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

	System::init();
	System::create(&process1);
	System::create(&process2);

	while (1)
	{
		System::schedule();
	}
}