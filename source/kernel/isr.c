#include <idt.c>

/* This defines what the stack looks like after an ISR was running */
struct regs
{
	unsigned int gs, fs, es, ds;						 /* pushed the segs last */
	unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax; /* pushed by 'pusha' */
	unsigned int int_no, err_code;						 /* our 'push byte #' and ecodes do this */
	unsigned int eip, cs, eflags, useresp, ss;			 /* pushed by the processor automatically */
};

/* These are function prototypes for all of the exception
 *  handlers: The first 32 entries in the IDT are reserved
 *  by Intel, and are designed to service exceptions! */
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
const char* exception_messages[32] = {
	"Division by zero",
	"Single-step Interrupt",
	"NMI",
	"Breakpoint",
	"Overflow",
	"Bound Range Exceeded",
	"Invalid Opcode",
	"Coprocessor not available",
	"Double fault",
	"Coprocessor segment overrun",
	"Invalid task state segment",
	"Segment not present",
	"Stack segment fault",
	"General protection fault",
	"Page fault",
	"Reserved",
	"x87 floating point exception",
	"Alignment check",
	"Machine check",
	"SIMD floating point exception",
	"Virtualization exception",
	"Control protection exception",
	"reserved",
	"IRQ 0-7",
	"IRQ 8-15",
};

void init_isr()
{
	println("Intializing interrupts...");

	idt_set_gate(0, (uint32_t)&isr0, 0x08, 0x8E);
	println("Set %s (%i) interrupt.", exception_messages[0], 0);
}

/* All of our Exception handling Interrupt Service Routines will
 *  point to this function. This will tell us what exception has
 *  happened! Right now, we simply halt the system by hitting an
 *  endless loop. All ISRs disable interrupts while they are being
 *  serviced as a 'locking' mechanism to prevent an IRQ from
 *  happening and messing up kernel data structures */
void fault_handler(struct regs* r)
{
	/* Is this a fault whose number is from 0 to 31? */
	if (r->int_no < 32)
	{
		/* Display the description for the Exception that occurred.
		 *  In this tutorial, we will simply halt the system using an
		 *  infinite loop */
		println(exception_messages[r->int_no]);
		println(" Exception. System Halted!");
		while (1)
		{
			// Infinite loop
		}
	}
}