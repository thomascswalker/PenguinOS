// https://www.thejat.in/learn/implement-gdt-in-multiboot-bootloader

#include <io.h>

#define GDT_ENTRY_COUNT 3

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

gdt_entry_t g_gdt[GDT_ENTRY_COUNT];
gdt_ptr_t	g_gdt_ptr;

// Defined in gdt.asm
// Reloads the segment registers.
extern void gdt_flush(uint32_t gdt_ptr);

static void gdt_set_gate(int32_t index, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity)
{
	g_gdt[index].base_low = (base & 0xFFFF);
	g_gdt[index].base_middle = (base >> 16) & 0xFF;
	g_gdt[index].base_high = (base >> 24) & 0xFF;

	g_gdt[index].limit_low = (limit & 0xFFFF);

	g_gdt[index].granularity = (limit >> 16) & 0x0F;
	g_gdt[index].granularity |= granularity & 0xF0;

	g_gdt[index].access = access;
}

void init_gdt()
{
	println("Initializing GDT...");

	g_gdt_ptr.limit = (sizeof(gdt_entry_t) * GDT_ENTRY_COUNT) - 1;
	g_gdt_ptr.base = (uint32_t)&g_gdt;

	println("GDT ptr limit = %i", g_gdt_ptr.limit);
	println("GDT ptr base = %i", g_gdt_ptr.base);

	gdt_set_gate(0, 0, 0, 0, 0); // NULL gate
	gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
	gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);

	println("GDT[1] %i %i %i %i %i %i",								 //
		g_gdt[1].base_low, g_gdt[1].base_middle, g_gdt[1].base_high, //
		g_gdt[1].limit_low, g_gdt[1].granularity, g_gdt[1].access);
	println("GDT[2] %i %i %i %i %i %i",								 //
		g_gdt[2].base_low, g_gdt[2].base_middle, g_gdt[2].base_high, //
		g_gdt[2].limit_low, g_gdt[2].granularity, g_gdt[2].access);

	// Enter gdt.s
	println("Entering gdt_flush with ptr %i", g_gdt_ptr);
	// gdt_flush((uint32_t)&g_gdt_ptr);

	println("GDT initialized.");
}
