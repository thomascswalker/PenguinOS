#include <idt.c>
#include <isr.h>
#include <pic.c>

isr_handler_t interrupt_handlers[256];

void register_interrupt_handler(uint8_t interrupt, isr_handler_t handler)
{
	interrupt_handlers[interrupt] = handler;
}

void isr_handler(registers_t regs)
{
	if (regs.int_no == GENERAL_PROTECTION_FAULT)
	{
		println("General protection fault. Code %i", regs.err_code);
	}
	if (interrupt_handlers[regs.int_no])
	{
		interrupt_handlers[regs.int_no](regs);
	}
}

void irq_handler(registers_t regs)
{
	if (regs.int_no >= 40)
	{
		outb(SLAVE_COMMAND, PIC_RESET);
	}

	outb(MASTER_COMMAND, PIC_RESET);

	if (interrupt_handlers[regs.int_no])
	{
		interrupt_handlers[regs.int_no](regs);
	}
}