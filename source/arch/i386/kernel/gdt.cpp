#include <gdt.h>

void GDT::init()
{
	ptr.limit = (sizeof(GDTEntry) * GDT_ENTRY_COUNT) - 1;
	ptr.base = (uint32_t)&entries;

	setGate(0, 0, 0, 0, 0);				   // Null segment
	setGate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // Code segment
	setGate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // Data segment
	setGate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // User mode code segment
	setGate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); // User mode data segment

	loadGDT(&ptr);
}

void GDT::setGate(int32_t entry, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran)
{
	entries[entry].baseLow = (base & 0xFFFF);
	entries[entry].baseMiddle = (base >> 16) & 0xFF;
	entries[entry].baseHigh = (base >> 24) & 0xFF;

	entries[entry].limitLow = (limit & 0xFFFF);
	entries[entry].granularity = (limit >> 16) & 0x0F;

	entries[entry].granularity = gran;
	entries[entry].access = access;
}
