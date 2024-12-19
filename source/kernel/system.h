#pragma once

#include <types.h>

static inline uint8 inb(uint16 port)
{
	unsigned char ret;
	__asm__ volatile("inb %w1, %b0" : "=a"(ret) : "Nd"(port) : "memory");
	return ret;
};

static inline void outb(uint16 port, uint8 data)
{
	__asm__ volatile("outb %b0, %w1" : : "a"(data), "Nd"(port) : "memory");
}