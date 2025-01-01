#pragma once

#include <types.h>

#define asm __asm__ volatile

struct registers
{
	uint32_t cr2;
	uint32_t ds;
	uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
	uint32_t int_no, err_code;
	uint32_t iret_rip, iret_cs, iret_flags, iret_rsp,
		iret_ss; // Automatically popped from the stack when IRET is called
};
typedef struct registers registers_t;

// Function pointer to an IRQ callback
typedef void (*handler_t)(registers_t);

enum privelege_level
{
	RING0,
	RING1,
	RING2,
	RING3
};

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

static inline void disable_interrupts()
{
	asm("cli");
}

static inline void enable_interrupts()
{
	asm("sti");
}

static inline void halt()
{
	asm("hlt");
}

static inline void exit()
{
	outw(0x604, 0x2000); // QEMU
}