/*
Main entry point into PengOS. Initializes the kernel.
*/

#include <gdt.h>
#include <keyboard.h>
#include <multiboot.h>
#include <pit.h>
#include <pmm.h>

EXTERN void kmain(MultibootInfo* mbi, uint32_t magic)
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

	initMultiboot(mbi, &heapAddr, &heapSize);
	uint32_t heapStart = (uintptr_t)&kernelEnd + heapAddr;

	PMM::init(heapStart, heapSize);
	void* p0 = PMM::kmalloc(400);
	debugx(p0);
	void* p1 = PMM::kmalloc(2000);
	debugx(p1);
	void* p2 = PMM::kmalloc(200);
	debugx(p2);

	while (1)
	{
	}
}