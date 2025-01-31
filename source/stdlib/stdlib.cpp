#include <stdio.h>
#include <stdlib.h>
#include <syscall.h>

void itoa(char* buffer, uint32_t value, uint32_t base)
{
	uint32_t temp = value;
	uint32_t i = 0;

	do
	{
		temp = value % base;
		buffer[i++] = (temp < 10) ? (temp + '0') : (temp + 'a' - 10);
	}
	while (value /= base);

	buffer[i--] = 0;

	for (uint32_t j = 0; j < i; j++, i--)
	{
		temp = buffer[j];
		buffer[j] = buffer[i];
		buffer[i] = temp;
	}
}

// Allocate unitialized memory, uses syscall
void* malloc(const uint32_t size)
{
	void* ptr = 0;
	asm("int $0x80" : "=a"(ptr) : "a"(SYSCALL_MALLOC), "b"(size));
	debug("Malloc %d: %x", size, ptr);
	return ptr;
}

// Free memory, uses syscall
void free(const void* ptr)
{
	asm("int $0x80" ::"a"(SYSCALL_FREE), "b"(ptr)); // Free ptr in memory area
	ptr = nullptr;
}