/*
Main entry point into PengOS. Initializes the kernel.
*/

#include <cpuid.h>
#include <gdt.h>
#include <keyboard.h>
#include <malloc.h>
#include <memory.h>
#include <multiboot.h>
#include <syscall.h>
#include <timer.h>

void* ptov(void* ptr)
{
	if ((uintptr_t)ptr > VIRTUAL_START)
	{
		return ptr;
	}
	return (void*)((uintptr_t)ptr + VIRTUAL_START);
}

void kernel_main(uint32_t magic, multiboot_info_t* info)
{
	init_terminal();
	println("Welcome to PengOS!");

	if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
	{
		panic("Invalid multiboot magic. Got %x, wanted %x.", magic, MULTIBOOT_BOOTLOADER_MAGIC);
	}

	init_gdt();
	init_idt();
	init_timer();
	init_keyboard();

	// Remap the multiboot info struct from physical
	// to virtual (essentially offsetting 0xC0000000).
	info = (multiboot_info_t*)ptov((void*)info);

	void* malloc_memory_start;
	void* malloc_memory_end;

	// Find the largest memory map and use this as the malloc
	// memory space.
	multiboot_mmap_entry_t* mmap;			   // Current memory map to check
	multiboot_mmap_entry_t* largest = nullptr; // Largest memory map
	for (mmap = (multiboot_mmap_entry_t*)ptov((void*)info->mmap_addr);
		 (uintptr_t)mmap < (uintptr_t)ptov((void*)info->mmap_addr + info->mmap_length);
		 mmap = (multiboot_mmap_entry_t*)((uintptr_t)mmap + mmap->size + sizeof(mmap->size)))
	{
		if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE
			&& (largest == nullptr || mmap->len_low > largest->len_low))
		{
			largest = mmap;
		}
	}

	if (largest != nullptr)
	{
		malloc_memory_start = (void*)(uintptr_t)largest->addr_low;
		malloc_memory_end = malloc_memory_start + largest->len_low;
	}
	uint32_t malloc_mem_size = malloc_memory_end - malloc_memory_start;
	// // Initialize physical memory.
	init_pmm(malloc_memory_end, malloc_mem_size);
	// // Initialize virtual memory.
	// init_vmm();

	// // After memory is intialized, enable interrupts (page fault, etc.)
	enable_interrupts();

	// // Test malloc
	// void* ptr = malloc(10000);
	// debug("ptr: %x", (uint32_t*)ptr);
	// free(ptr);

	while (true)
	{
	}
}