#include <io.h>

#define IDT_ENTRY_COUNT 256

struct idt_entry
{
	uint16_t base_low;
	uint16_t sel;
	uint8_t	 always0;
	uint8_t	 flags;
	uint16_t base_high;
} __attribute__((packed));
typedef struct idt_entry idt_entry_t;

struct idt_ptr
{
	uint16_t limit;
	uint32_t base;
} __attribute__((packed));
typedef struct idt_ptr idt_ptr_t;

idt_entry_t idt[IDT_ENTRY_COUNT];
idt_ptr_t	idtp;

extern void idt_load();
void		idt_set_gate(uint8_t index, uint32_t base, uint16_t sel, uint8_t flags)
{
	println("Setting IDT Gate ISR%i: [Base=%i, Sel=%i, Flags=%i]", index, base, sel, flags);
	idt[index].base_low = (uint16_t)(base >> 8);
	idt[index].base_high = (uint16_t)(base >> 16);
	idt[index].always0 = 0;
	idt[index].sel = sel;
	idt[index].flags = flags;
}
void init_idt()
{
	idtp.limit = (sizeof(idt_entry_t) * IDT_ENTRY_COUNT) - 1;
	idtp.base = (uint32_t)&idt;

	memset((uint8_t*)&idt, 0, sizeof(idt_entry_t) * IDT_ENTRY_COUNT);

	idt_load();
}