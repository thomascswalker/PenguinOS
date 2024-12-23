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
void		init_idt()
{
	idtp.limit = (sizeof(idt_entry_t) * IDT_ENTRY_COUNT) - 1;
	idtp.base = (uint32_t)&idt;

	memset((uint8_t*)&idt, 0, sizeof(idt_entry_t) * IDT_ENTRY_COUNT);

	idt_load();
}