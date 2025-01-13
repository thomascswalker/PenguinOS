#pragma once

#include <sys.h>

#define IDT_ENTRY_COUNT 256

#define ISR0 0 // Divide by zero
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

#define IRQ0 32 // Timer
#define IRQ1 33 // Keyboard
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

#define DOUBLE_FAULT 0x08
#define GENERAL_PROTECTION_FAULT 0x0D
#define PAGE_FAULT 0x0E

struct IDTEntry
{
	uint16_t base_low;
	uint16_t sel;
	uint8_t	 always0;
	uint8_t	 flags;
	uint16_t base_high;
} __attribute__((packed));

struct IDTPtr
{
	uint16_t limit; // Size - 1
	uint32_t base;	// Pointer to the address of our
					// interrupt handler array
} __attribute__((packed));

static const char* idt_messages[] = {
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
	"Timer",						 // 32
	"Keyboard",						 // 33
};

namespace IDT
{
	void init();
	void setGate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);

	void registerInterruptHandler(uint32_t index, Handler handler);
	void unregisterInterruptHandler(uint32_t index);

	EXTERN void loadIDT(uint32_t);
	EXTERN void irqHandler(Registers regs);
	EXTERN void isrHandler(Registers regs);

	EXTERN void isr0();
	EXTERN void isr1();
	EXTERN void isr2();
	EXTERN void isr3();
	EXTERN void isr4();
	EXTERN void isr5();
	EXTERN void isr6();
	EXTERN void isr7();
	EXTERN void isr8();
	EXTERN void isr9();
	EXTERN void isr10();
	EXTERN void isr11();
	EXTERN void isr12();
	EXTERN void isr13();
	EXTERN void isr14();
	EXTERN void isr15();
	EXTERN void isr16();
	EXTERN void isr17();
	EXTERN void isr18();
	EXTERN void isr19();
	EXTERN void isr20();
	EXTERN void isr21();
	EXTERN void isr22();
	EXTERN void isr23();
	EXTERN void isr24();
	EXTERN void isr25();
	EXTERN void isr26();
	EXTERN void isr27();
	EXTERN void isr28();
	EXTERN void isr29();
	EXTERN void isr30();
	EXTERN void isr31();

	EXTERN void irq0();
	EXTERN void irq1();
	EXTERN void irq2();
	EXTERN void irq3();
	EXTERN void irq4();
	EXTERN void irq5();
	EXTERN void irq6();
	EXTERN void irq7();
	EXTERN void irq8();
	EXTERN void irq9();
	EXTERN void irq10();
	EXTERN void irq11();
	EXTERN void irq12();
	EXTERN void irq13();
	EXTERN void irq14();
	EXTERN void irq15();

	void dumpRegisters(Registers* reg);
} // namespace IDT
