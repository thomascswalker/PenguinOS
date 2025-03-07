#pragma once

#include <ctype.h>

#define KB(x) (x * 0x400)
#define MB(x) (x * 0x100000)
#define GB(x) (x * 0x40000000)
#define asm __asm__ __volatile__

struct Registers
{
	/* Pushed by the ISR stub. */
	uint64_t r15;
	uint64_t r14;
	uint64_t r13;
	uint64_t r12;
	uint64_t r11;
	uint64_t r10;
	uint64_t r9;
	uint64_t r8;
	uint64_t rdi;
	uint64_t rsi;
	uint64_t rbp;
	uint64_t rdx;
	uint64_t rcx;
	uint64_t rbx;
	uint64_t rax;

	/* Pushed by the ISR wrapper function (if you choose to push these). */
	uint64_t int_no;
	uint64_t err_code;

	/* Pushed automatically by the CPU on interrupt/exception. */
	uint64_t rip;
	uint64_t cs;
	uint64_t rflags;
	uint64_t rsp;
	uint64_t ss;
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

// Reads two bytes of data from the specified IO port
static inline uint16_t inw(uint16_t port)
{
	uint16_t ret;
	asm("inw %1, %0" : "=a"(ret) : "Nd"(port));
	return ret;
};

// Outputs two bytes of data to a specified IO port
static inline void outw(uint16_t port, uint16_t data)
{
	asm("outw %1, %0" : : "Nd"(port), "a"(data));
}

static inline void disableInterrupts() { asm("cli"); }

static inline void enableInterrupts() { asm("sti"); }

static inline void syspause()
{
	while (true)
	{
	}
}

static inline void syshalt()
{
	asm("hlt");
	while (true)
	{
	}
}

static inline void sysexit()
{
	outw(0x604, 0x2000); // QEMU
}

static inline void invalidate(uint64_t vaddr) { asm("invlpg %0" ::"m"(vaddr)); }

void sleep(uint32_t seconds);
