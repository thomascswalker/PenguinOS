#pragma once

#include "types.h"

static inline uint8 inb(uint16 _port)
{
	unsigned char rv;
	__asm__ volatile("inb %w1, %b0" : "=a"(rv) : "Nd"(_port) : "memory");
	return rv;
};

static inline void outb(uint16 _port, uint8 _data)
{
	__asm__ volatile("outb %b0, %w1" : : "a"(_data), "Nd"(_port) : "memory");
}