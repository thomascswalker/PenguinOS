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
	GDT::init();
	IDT::init();
	PIT::init();
	Keyboard::init();

	// Once everything is initialized, enable interrupts.
	enableInterrupts();

	Paging::init();

	println("Welcome to PengOS!");

	while (1)
	{
	}
}