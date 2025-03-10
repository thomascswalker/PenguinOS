#include <cstdlib.h>
#include <memory.h>
#include <stdio.h>
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

// Allocates size `bytes` of uninitialized storage.
void* std::malloc(const uint32_t size) { return Memory::kmalloc(size); }

// Free memory, uses syscall
void std::free(void* ptr) { Memory::kfree(ptr); }

void* std::realloc(void* ptr, const uint32_t size) { return Memory::krealloc(ptr, size); }
