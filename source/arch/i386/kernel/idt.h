#pragma once

#include <sys.h>

#define DIVIDE_BY_ZERO 0x00
#define INVALID_OPCODE 0x06
#define DOUBLE_FAULT 0x08
#define GENERAL_PROTECTION_FAULT 0x0D
#define PAGE_FAULT 0x0E

#define TIMER 0x20
#define KEYBOARD 0x21
#define SYSTEM_CALL 0x80
#define ATA 0x2E

#define IDT_ENTRY_COUNT 256

#define ISR0 DIVIDE_BY_ZERO
#define ISR1 1
#define ISR2 2
#define ISR3 3
#define ISR4 4
#define ISR5 5
#define ISR6 6
#define ISR7 7
#define ISR8 DOUBLE_FAULT
#define ISR9 9
#define ISR10 10
#define ISR11 11
#define ISR12 12
#define ISR13 GENERAL_PROTECTION_FAULT
#define ISR14 PAGE_FAULT
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

#define ISR128 SYSTEM_CALL

#define IRQ0 TIMER
#define IRQ1 KEYBOARD
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
#define IRQ14 ATA
#define IRQ15 47

struct IDTEntry
{
	uint16_t baseLow;
	uint16_t sel;
	uint8_t	 always0;
	uint8_t	 flags;
	uint16_t baseHigh;
} __attribute__((packed));

struct IDTPtr
{
	uint16_t limit; // Size - 1
	uint32_t base;	// Pointer to the address of our
					// interrupt handler array
} __attribute__((packed));

namespace IDT
{
	void init();
	void setGate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);

	void registerInterruptHandler(uint32_t index, Handler handler);
	void unregisterInterruptHandler(uint32_t index);

	EXTERN void loadIDT(uint32_t);
	EXTERN void irqHandler(CPUState *regs);
	EXTERN void isrHandler(CPUState *regs);

	void handleGeneralProtectionFault(CPUState* regs);
	void handlePageFault(CPUState* regs);

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

	EXTERN void isr128();

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

	void dumpCPUState(CPUState* reg);
} // namespace IDT
