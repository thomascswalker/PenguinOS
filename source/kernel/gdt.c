// https://www.thejat.in/learn/implement-gdt-in-multiboot-bootloader
#pragma once

#include <stdio.c>

#define GDT_ENTRY_COUNT 5

struct gdt_entry
{
	uint16_t limit_low;
	uint16_t base_low;
	uint8_t	 base_middle;
	uint8_t	 access;
	uint8_t	 granularity;
	uint8_t	 base_high;
} __attribute__((packed));
typedef struct gdt_entry gdt_entry_t;

struct gdt_ptr
{
	uint16_t limit;
	uint32_t base;
} __attribute__((packed));
typedef struct gdt_ptr gdt_ptr_t;

gdt_entry_t gdt_entries[GDT_ENTRY_COUNT];
gdt_ptr_t	gdt_ptr;

extern void load_gdt(gdt_ptr_t* gdt_ptr);

static void gdt_set_gate(int32_t entry, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran)
{
	gdt_entries[entry].base_low = (base & 0xFFFF);
	gdt_entries[entry].base_middle = (base >> 16) & 0xFF;
	gdt_entries[entry].base_high = (base >> 24) & 0xFF;

	gdt_entries[entry].limit_low = (limit & 0xFFFF);
	gdt_entries[entry].granularity = (limit >> 16) & 0x0F;

	gdt_entries[entry].granularity = gran;
	gdt_entries[entry].access = access;
}

void init_gdt()
{
	info("Initializing GDT...");

	gdt_ptr.limit = (sizeof(gdt_entry_t) * GDT_ENTRY_COUNT) - 1;
	gdt_ptr.base = (uint32_t)&gdt_entries;

	gdt_set_gate(0, 0, 0, 0, 0);				// Null segment
	gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // Code segment
	gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // Data segment
	gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // User mode code segment
	gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); // User mode data segment

	load_gdt(&gdt_ptr);

	success("GDT initialized.");
}
