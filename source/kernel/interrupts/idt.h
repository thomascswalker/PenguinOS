#include <system.h>

#define IDT_ENTRY_COUNT 256
#define IRQ_ENTRY_COUNT 16

#define PIC0_COMMAND 0x20
#define PIC0_DATA 0x21
#define PIC1_COMMAND 0xA0
#define PIC1_DATA 0xA1

enum idt_gate : uint8_t
{
	TASK_GATE = 0x05,
	INT_GATE_16 = 0x06,
	TRAP_GATE_16 = 0x07,
	INT_GATE_32 = 0x0E,
	TRAP_GATE_32 = 0x0F
};
typedef enum idt_gate idt_gate_t;

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
	uint16_t size; // Size - 1
	uint32_t addr; // Pointer to the address of our IDT array
} __attribute__((packed));
typedef struct idt_ptr idt_ptr_t;

static void* irq_routines[IRQ_ENTRY_COUNT] = {
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
};

void init_idt();
void set_idt_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);

extern void isr_handler(registers_t regs);

extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();

extern void isr128(); // System calls
extern void isr177(); // System calls

void		irq_install_handler(uint32_t irq, handler_t handler);
void		irq_uninstall_handler(uint32_t irq);
extern void irq_handler(registers_t regs);

extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();
