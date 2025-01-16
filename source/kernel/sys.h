#pragma once

#include <ctype.h>

#define KB 0x400
#define MB 0x100000
#define GB 0x40000000
#define asm __asm__ volatile

struct Registers
{
	/* Pushed by common stub. */
	uint32_t ds, edi, esi, ebp, esp, ebx, edx, ecx, eax;
	/* Pushed by wrapper function. */
	uint32_t int_no, err_code;
	/* Pushed by interrupt. */
	uint32_t eip, cs, eflags, useresp, ss;
};

// Function pointer to an IRQ callback
typedef void (*Handler)(Registers);

// Reads a byte of data from the specified IO port
static inline uint8_t inb(uint16_t port)
{
	uint8_t ret;
	asm("inb %1, %0" : "=a"(ret) : "Nd"(port));
	return ret;
};

// Outputs a byte of data to a specified IO port
static inline void outb(uint16_t port, uint8_t data)
{
	asm("outb %1, %0" : : "Nd"(port), "a"(data));
}

static inline void outw(uint16_t port, uint16_t data)
{
	asm("outw %1, %0" : : "Nd"(port), "a"(data));
}

static inline void disableInterrupts()
{
	asm("cli");
}

static inline void enableInterrupts()
{
	asm("sti");
}

static inline void pause()
{
	while (true)
	{
	}
}

static inline void halt()
{
	asm("hlt");
	while (true)
	{
	}
}

static inline void exit()
{
	outw(0x604, 0x2000); // QEMU
}

static inline void invalidate(uint32_t vaddr)
{
	asm("invlpg %0" ::"m"(vaddr));
}

void sleep(uint32_t seconds);