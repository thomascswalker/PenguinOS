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
#include <testProcess.h>

static uint32_t i = 0;
static uint32_t j = 0;

void proc()
{
	while (1)
	{
		printf("Hello from process %d\n", i++);
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
	Scheduler::init();
	FileSystem::init();
	CMD::init();

	println("Welcome to Penguin OS!");

	Scheduler::create(&proc);

	while (1)
	{
		// Scheduler::schedule();
	}
}