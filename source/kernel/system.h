#pragma once

#include <types.h>

#define ASM __asm__

struct registers
{
	uint32_t cr2;
	uint32_t ds;
	uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
	uint32_t int_no, err_code;
	uint32_t eip, csm, eflags, useresp, ss;
};
typedef struct registers registers_t;
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
	ASM volatile("inb %w1, %b0" : "=a"(ret) : "Nd"(port) : "memory");
	return ret;
};

// Outputs a byte of data to a specified IO port
static inline void outb(uint16_t port, uint8_t data)
{
	ASM volatile("outb %1, %0" : : "Nd"(port), "a"(data));
}

static inline void disable_interrupts()
{
	ASM volatile("cli");
}

static inline void enable_interrupts()
{
	ASM volatile("sti");
}

static inline void halt()
{
	ASM volatile("hlt");
}