#pragma once

#include <ctype.h>

#define KB(x) (x * 0x400)
#define MB(x) (x * 0x100000)
#define GB(x) (x * 0x40000000)
#define asm __asm__ __volatile__

#define SEG_KERNEL_CODE 0x08
#define SEG_KERNEL_DATA 0x10
#define SEG_USER_CODE 0x1B
#define SEG_USER_DATA 0x23

struct CPUState
{
	int32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
	int32_t intNo, errCode;
	int32_t eip, cs, eFlags, userEsp, ss;
};

// Function pointer to an IRQ callback
typedef void (*Handler)(CPUState*);

// Reads a byte of data from the specified IO port
static uint8_t inb(uint16_t port)
{
	uint8_t ret;
	asm("inb %1, %0" : "=a"(ret) : "Nd"(port));
	return ret;
};

// Outputs a byte of data to a specified IO port
static void outb(uint16_t port, uint8_t data)
{
	asm("outb %1, %0" : : "Nd"(port), "a"(data));
}

// Reads two bytes of data from the specified IO port
static uint16_t inw(uint16_t port)
{
	uint16_t ret;
	asm("inw %1, %0" : "=a"(ret) : "Nd"(port));
	return ret;
};

// Outputs two bytes of data to a specified IO port
static void outw(uint16_t port, uint16_t data)
{
	asm("outw %1, %0" : : "Nd"(port), "a"(data));
}

static void disableInterrupts() { asm("cli"); }

static void enableInterrupts() { asm("sti"); }

static void syspause()
{
	while (true)
	{
	}
}

static void syshalt()
{
	asm("hlt");
	while (true)
	{
	}
}

static void sysexit()
{
	outw(0x604, 0x2000); // QEMU
}

static void invalidate(uint32_t vaddr) { asm("invlpg %0" ::"m"(vaddr)); }
