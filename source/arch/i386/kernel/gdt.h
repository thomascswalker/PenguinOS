// https://www.thejat.in/learn/implement-gdt-in-multiboot-bootloader
#pragma once

#include <cstdio.h>

#define GDT_ENTRY_COUNT 6

struct GDTEntry
{
	uint16_t limitLow;
	uint16_t baseLow;
	uint8_t	 baseMiddle;
	uint8_t	 access;
	uint8_t	 granularity;
	uint8_t	 baseHigh;
} __attribute__((packed));

struct GDTPtr
{
	uint16_t limit; // Size
	uint32_t base;	// Address
} __attribute__((packed));

namespace GDT
{
	void		init();
	void		setGate(int32_t entry, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);
	EXTERN void loadGDT(GDTPtr* gdtPtr); // Defined in gdt.s
} // namespace GDT
