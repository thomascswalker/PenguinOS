#pragma once

#include <sys.h>

#define IDT_ENTRY_COUNT 256

#define ISR0 0
#define ISR1 1
#define ISR2 2
#define ISR3 3
#define ISR4 4
#define ISR5 5
#define ISR6 6
#define ISR7 7
#define ISR8 8
#define ISR9 9
#define ISR10 10
#define ISR11 11
#define ISR12 12
#define ISR13 13
#define ISR14 14
#define ISR15 15
#define ISR16 16
#define ISR17 17
#define ISR18 18
#define ISR19 19
#define ISR20 20
#define ISR21 21
#define ISR22 22
#define ISR23 23
#define ISR24 24
#define ISR25 25
#define ISR26 26
#define ISR27 27
#define ISR28 28
#define ISR29 29
#define ISR30 30
#define ISR31 31

#define IRQ0 32
#define IRQ1 33
#define IRQ2 34
#define IRQ3 35
#define IRQ4 36
#define IRQ5 37
#define IRQ6 38
#define IRQ7 39
#define IRQ8 40
#define IRQ9 41
#define IRQ10 42
#define IRQ11 43
#define IRQ12 44
#define IRQ13 45
#define IRQ14 46
#define IRQ15 47

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

handler_t interrupt_handlers[256];

static const char* isr_messages[32] = {
	"Division by zero",				 // 0
	"Single-step Interrupt",		 // 1
	"NMI",							 // 2
	"Breakpoint",					 // 3
	"Overflow",						 // 4
	"Bound Range Exceeded",			 // 5
	"Invalid Opcode",				 // 6
	"Coprocessor not available",	 // 7
	"Double fault",					 // 8
	"Coprocessor segment overrun",	 // 9
	"Invalid task state segment",	 // 10
	"Segment not present",			 // 11
	"Stack segment fault",			 // 12
	"General protection fault",		 // 13
	"Page fault",					 // 14
	"Unknown",						 // 15
	"x87 floating point exception",	 // 16
	"Alignment check",				 // 17
	"Machine check",				 // 18
	"SIMD floating point exception", // 19
	"Virtualization exception",		 // 20
	"Control protection exception",	 // 21
	"Reserved",						 // 22
	"Reserved",						 // 23
	"Reserved",						 // 24
	"Reserved",						 // 25
	"Reserved",						 // 26
	"Reserved",						 // 27
	"Reserved",						 // 28
	"Reserved",						 // 29
	"Reserved",						 // 30
	"Reserved",						 // 31
};

static const char* irq_messages[2] = {
	"Timer",	// 32
	"Keyboard", // 33
};

void init_idt();
void set_idt_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);

void register_interrupt_handler(uint32_t index, handler_t handler);
void unregister_interrupt_handler(uint32_t index);

extern void irq_handler(registers_t regs);
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

void dump_registers(registers_t* reg);
