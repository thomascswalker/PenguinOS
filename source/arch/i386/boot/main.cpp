/*
Main entry point into PenguinOS.
*/

#include <filesystem.h>
#include <gdt.h>
#include <keyboard.h>
#include <memory.h>
#include <multiboot.h>
#include <pit.h>
#include <shell.h>

EXTERN typedef void (*ConstructorType)();
EXTERN ConstructorType __init_array_start[];
EXTERN ConstructorType __init_array_end[];
EXTERN void			   callConstructors()
{
	for (ConstructorType* func = __init_array_start; func < __init_array_end; ++func)
	{
		(*func)();
	}
}

// TODO: Replace __cxa_atexit and __dso_handle with proper implementations
// to handle global destructors.
EXTERN int __cxa_atexit(void (*)(void*), void*, void*)
{
	// Not registering destructors in a freestanding environment.
	return 0;
}
EXTERN void* __dso_handle = nullptr;

EXTERN void kmain(MultibootInfo* info, uint32_t magic)
{
	callConstructors();

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
	println("Welcome to PengOS");

	Memory::init(start, size);
	FileSystem::init();

	while (1)
	{
	}
}