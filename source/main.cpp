/*
Main entry point into PengOS. Initializes the kernel.
*/

#include <gdt.h>
#include <keyboard.h>
#include <multiboot.h>
#include <pit.h>
#include <pmm.h>

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
	uint32_t heapSize = 0;
	uint32_t heapAddr = 0;
	initMultiboot(info, &heapAddr, &heapSize);
	uint32_t heapStart = (uintptr_t)&kernelEnd + heapAddr;

	PMM::init(heapStart, heapSize);
	while (1)
	{
	}
}