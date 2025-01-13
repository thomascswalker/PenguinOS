/*
Main entry point into PengOS. Initializes the kernel.
*/

#include <gdt.h>
#include <heap.h>
#include <keyboard.h>
#include <memory.h>
#include <multiboot.h>
#include <pit.h>

EXTERN void kmain(multiboot_info_t* boot_info)
{
	VGA::init();
	println("Welcome to PengOS!");

	GDT::init();
	IDT::init();
	PIT::init();
	Keyboard::init();

	// Once everything is initialized, enable interrupts.
	enableInterrupts();

	uint32_t memorySize = KHEAP_INITIAL_SIZE;
	PMM::init(memorySize);

	uint32_t freeCount = PMM::getFreeBlockCount();
	debugi(freeCount);
	void* ptr = Heap::kmalloc(KHEAP_MAX_SIZE);
	debugx(ptr);
	freeCount = PMM::getFreeBlockCount();
	debugi(freeCount);
	Heap::kfree(ptr);
	freeCount = PMM::getFreeBlockCount();
	debugi(freeCount);

	while (1)
	{
	}
}