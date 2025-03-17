#include <cstdlib.h>
#include <memory.h>
#include <stdio.h>
#include <syscall.h>

/**
 * Converts an integer `value` to a null-terminated string
 * using the specified `base` and stores the result in the
 * array given by `str` parameter.
 *
 * @param str Array in memory where to store the
 * resulting null-terminated string.
 * @param value Value to be converted to a string.
 * @param base Numerical base used to represent the value as
 *  a string, between 2 and 36, where 10 means decimal base,
 *  16 hexadecimal, 8 octal, and 2 binary.
 */
void itoa(char* str, uint32_t value, uint32_t base)
{
	uint32_t temp = value;
	uint32_t i = 0;

	do
	{
		temp = value % base;
		str[i++] = (temp < 10) ? (temp + '0') : (temp + 'a' - 10);
	}
	while (value /= base);

	str[i--] = 0;

	for (uint32_t j = 0; j < i; j++, i--)
	{
		temp = str[j];
		str[j] = str[i];
		str[i] = temp;
	}
}

/**
 * Allocates size `bytes` of uninitialized storage.
 *
 * @param size Number of bytes to allocate.
 */
void* std::malloc(const uint32_t size) { return Memory::kmalloc(size); }

/**
 * Deallocates the space previously allocated by `std::malloc` or `std::realloc`.
 *
 * @param ptr Pointer to the memory area to be deallocated.
 */
void std::free(void* ptr) { Memory::kfree(ptr); }

/**
 * Reallocates the given area of memory previously allocated by `std::malloc` or `std::realloc` to a
 * new size.
 *
 * @param ptr Pointer to the memory area to be reallocated.
 * @param size New size of the memory area.
 */
void* std::realloc(void* ptr, const uint32_t size) { return Memory::krealloc(ptr, size); }
