#include <stdio.h>
#include <stdlib.h>
#include <syscall.h>

// Allocate unitialized memory, uses syscall
void* malloc(const uint32_t size)
{
	void* ptr = 0;
	__asm__ __volatile__("int $0x80" : "=d"(ptr) : "a"(SYSCALL_MALLOC), "b"(size));
	if (!ptr)
	{
		panic("Failed to initialize memory of size %d.", size);
	}
	return ptr;
}

// Free memory, uses syscall
void free(const void* ptr)
{
	asm("int $0x80" ::"a"(SYSCALL_FREE), "b"(ptr)); // Free ptr in memory area
}