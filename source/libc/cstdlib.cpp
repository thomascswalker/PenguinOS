#include <cstdio.h>
#include <cstdlib.h>
#include <memory.h>
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
void* malloc(const uint32_t size) { return Memory::kmalloc(size); }

/**
 * Deallocates the space previously allocated by `malloc` or `realloc`.
 *
 * @param ptr Pointer to the memory area to be deallocated.
 */
void free(void* ptr) { Memory::kfree(ptr); }

/**
 * Reallocates the given area of memory previously allocated by `malloc` or `realloc` to a
 * new size.
 *
 * @param ptr Pointer to the memory area to be reallocated.
 * @param size New size of the memory area.
 */
void* realloc(void* ptr, const uint32_t size) { return Memory::krealloc(ptr, size); }

// Calls sys_sleep in syscall.h
void sleep(uint32_t seconds)
{
	// Convert seconds to milliseconds
	uint32_t ticks = seconds * 1000;

	// Execute interrupt 128 (0x80) to trigger a system call. Pass in
	// The sleep code (12) and the number of ticks to sleep for.
	asm("int $0x80" : : "a"(SYSCALL_SLEEP), "b"(ticks));
}

/**
 * Opens a file specified by the given filename.
 */
int32_t open(const char* filename)
{
	int32_t result = 0;
	asm("int $0x80" : "=a"(result) : "0"(SYSCALL_OPEN), "b"(filename));
	return result;
}

size_t read(int32_t fd, void* buffer, size_t size)
{
	int32_t result = 0;
	asm("int $0x80" : "=a"(result) : "0"(SYSCALL_READ), "b"(fd), "c"(buffer), "d"(size));
	return result;
}

void close(int32_t fd) { asm("int $0x80" ::"a"(SYSCALL_CLOSE), "b"(fd)); }

void stat(int32_t fd, void* buffer) { asm("int $0x80" ::"a"(SYSCALL_FSTAT), "b"(fd), "c"(buffer)); }
