/*
Main entry point into PengOS. Initializes the kernel.
*/

#include <gdt.h>
#include <keyboard.h>
#include <multiboot.h>
#include <paging.h>
#include <pit.h>
#include <pmm.h>

static uint32_t heapSize = 0;
static uint32_t heapAddr = 0;

EXTERN void kmain(MultibootInfo* info, uint32_t magic)
{
	VGA::init();
	println("Welcome to PengOS!");

	GDT::init();
	IDT::init();
	PIT::init();
	Keyboard::init();

	// Once everything is initialized, enable interrupts.
	enableInterrupts();

	Multiboot::init(info, &heapAddr, &heapSize);

	PMM::init(heapAddr, heapSize);
	Paging::init();

	while (1)
	{
	}
}