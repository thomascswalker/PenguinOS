#include <gdt.h>

static GDTEntry g_entries[GDT_ENTRY_COUNT];
static GDTPtr	g_ptr;

void GDT::init()
{
	g_ptr.limit = (sizeof(GDTEntry) * GDT_ENTRY_COUNT) - 1;
	g_ptr.base = (uint32_t)&g_entries;

	setGate(0, 0, 0, 0, 0);				   // Null segment
	setGate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // Code segment
	setGate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // Data segment
	setGate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // User mode code segment
	setGate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); // User mode data segment

	loadGDT(&g_ptr);
}

void GDT::setGate(int32_t entry, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran)
{
	g_entries[entry].baseLow = (base & 0xFFFF);
	g_entries[entry].baseMiddle = (base >> 16) & 0xFF;
	g_entries[entry].baseHigh = (base >> 24) & 0xFF;

	g_entries[entry].limitLow = (limit & 0xFFFF);
	g_entries[entry].granularity = (limit >> 16) & 0x0F;

	g_entries[entry].granularity = gran;
	g_entries[entry].access = access;
}
