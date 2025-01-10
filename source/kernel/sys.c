#include <stdio.h>
#include <sys.h>
#include <syscall.h>

void sleep(uint32_t seconds)
{
	__asm__ __volatile__("int $0x80" : : "a"(SYSCALL_SLEEP), "b"(seconds * 1000));
}