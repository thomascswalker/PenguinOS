#pragma once

#include <memory.h>
#include <pmm.h>
#include <sys.h>

#define PAGE_SIZE 4096
#define VIRTUAL_START (void*)0xC0000000						 // 3GB
#define KHEAP_START VIRTUAL_START + (4 * KB)				 // Virtual start + 4KB
#define KHEAP_INITIAL_SIZE (48 * MB) - (4 * KB)				 // 48MB - 4KB
#define KHEAP_MIN_SIZE (4 * MB)								 // 4MB
#define KHEAP_MAX_SIZE KHEAP_INITIAL_SIZE					 // 48MB - 4KB
#define KHEAP_MAX_ADDRESS VIRTUAL_START + KHEAP_INITIAL_SIZE // 3GB + 48MB

namespace Heap
{
	void* kmalloc(uint32_t size);
	void  kfree(void* ptr);
}; // namespace Heap