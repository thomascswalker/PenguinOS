#pragma once

#include <string.h>

#define NULL 0
#define nullptr ((void*)0)

static void itoa(char* buffer, uint32_t value, uint32_t base)
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
static void* malloc(const uint32_t size)
{
	void* ptr = 0;

	// // Syscall 3
	// asm("int $0x80" ::"a"(3), "b"(size)); // Create ptr to memory area
	// asm("mov $0, %e%ax" : "=r"(ptr));	  // Return ptr to memory area

	return ptr;
}

// Free memory, uses syscall
static void free(const void* ptr)
{
	// asm("int $0x80" ::"a"(4), "b"(ptr)); // Free ptr in memory area
}