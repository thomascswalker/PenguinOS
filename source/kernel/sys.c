#include <stdio.h>
#include <sys.h>
#include <syscall.h>
#include <timer.h>

// Calls sys_sleep in syscall.h
void sleep(uint32_t seconds)
{
	// Convert seconds to milliseconds
	uint32_t ticks = seconds * 1000;

	// Execute interrupt 128 (0x80) to trigger a system call. Pass in
	// The sleep code (12) and the number of ticks to sleep for.
	__asm__ __volatile__("int $0x80" : : "a"(SYSCALL_SLEEP), "b"(ticks));
}