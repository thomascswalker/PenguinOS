#pragma once

#include <types.h>

/* This defines what the stack looks like after an ISR was running */
struct registers
{
	unsigned int gs, fs, es, ds;						 /* pushed the segs last */
	unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax; /* pushed by 'pusha' */
	unsigned int int_no, err_code;						 /* our 'push byte #' and ecodes do this */
	unsigned int eip, cs, eflags, useresp, ss;			 /* pushed by the processor automatically */
};
typedef struct registers registers_t;

typedef void (*isr_handler_t)(registers_t);
void register_interrupt_handler(uint8_t interrupt, isr_handler_t handler);

extern void halt();

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