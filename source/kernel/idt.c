#include <idt.h>
#include <io.c>
#include <pic.c>

idt_entry_t idt_entries[IDT_ENTRY_COUNT];
idt_ptr_t	idtp;

extern void load_idt(uint32_t);

void init_idt()
{
	// Setup PIC
	info("Remapping IRQ table...");
	pic_remap();
	info("IRQ table remapped.");

	info("Initializing IDT...");

	idtp.size = sizeof(idt_entry_t) * IDT_ENTRY_COUNT - 1;
	idtp.addr = (uint32_t)&idt_entries;

	memset(&idt_entries, 0, sizeof(idt_entry_t) * IDT_ENTRY_COUNT);

	info("Setting ISR gates...");
	set_idt_gate(ISR0, (uint32_t)isr0, 0x08, 0x8E);
	set_idt_gate(ISR1, (uint32_t)isr1, 0x08, 0x8E);
	set_idt_gate(ISR2, (uint32_t)isr2, 0x08, 0x8E);
	set_idt_gate(ISR3, (uint32_t)isr3, 0x08, 0x8E);
	set_idt_gate(ISR4, (uint32_t)isr4, 0x08, 0x8E);
	set_idt_gate(ISR5, (uint32_t)isr5, 0x08, 0x8E);
	set_idt_gate(ISR6, (uint32_t)isr6, 0x08, 0x8E);
	set_idt_gate(ISR7, (uint32_t)isr7, 0x08, 0x8E);
	set_idt_gate(ISR8, (uint32_t)isr8, 0x08, 0x8E);
	set_idt_gate(ISR9, (uint32_t)isr9, 0x08, 0x8E);
	set_idt_gate(ISR10, (uint32_t)isr10, 0x08, 0x8E);
	set_idt_gate(ISR11, (uint32_t)isr11, 0x08, 0x8E);
	set_idt_gate(ISR12, (uint32_t)isr12, 0x08, 0x8E);
	set_idt_gate(ISR13, (uint32_t)isr13, 0x08, 0x8E);
	set_idt_gate(ISR14, (uint32_t)isr14, 0x08, 0x8E);
	set_idt_gate(ISR15, (uint32_t)isr15, 0x08, 0x8E);
	set_idt_gate(ISR16, (uint32_t)isr16, 0x08, 0x8E);
	set_idt_gate(ISR17, (uint32_t)isr17, 0x08, 0x8E);
	set_idt_gate(ISR18, (uint32_t)isr18, 0x08, 0x8E);
	set_idt_gate(ISR19, (uint32_t)isr19, 0x08, 0x8E);
	set_idt_gate(ISR20, (uint32_t)isr20, 0x08, 0x8E);
	set_idt_gate(ISR21, (uint32_t)isr21, 0x08, 0x8E);
	set_idt_gate(ISR22, (uint32_t)isr22, 0x08, 0x8E);
	set_idt_gate(ISR23, (uint32_t)isr23, 0x08, 0x8E);
	set_idt_gate(ISR24, (uint32_t)isr24, 0x08, 0x8E);
	set_idt_gate(ISR25, (uint32_t)isr25, 0x08, 0x8E);
	set_idt_gate(ISR26, (uint32_t)isr26, 0x08, 0x8E);
	set_idt_gate(ISR27, (uint32_t)isr27, 0x08, 0x8E);
	set_idt_gate(ISR28, (uint32_t)isr28, 0x08, 0x8E);
	set_idt_gate(ISR29, (uint32_t)isr29, 0x08, 0x8E);
	set_idt_gate(ISR30, (uint32_t)isr30, 0x08, 0x8E);
	set_idt_gate(ISR31, (uint32_t)isr31, 0x08, 0x8E);

	info("Setting IRQ gates...");
	set_idt_gate(IRQ0, (uint32_t)irq0, 0x08, 0x8E);
	set_idt_gate(IRQ1, (uint32_t)irq1, 0x08, 0x8E);
	set_idt_gate(IRQ2, (uint32_t)irq2, 0x08, 0x8E);
	set_idt_gate(IRQ3, (uint32_t)irq3, 0x08, 0x8E);
	set_idt_gate(IRQ4, (uint32_t)irq4, 0x08, 0x8E);
	set_idt_gate(IRQ5, (uint32_t)irq5, 0x08, 0x8E);
	set_idt_gate(IRQ6, (uint32_t)irq6, 0x08, 0x8E);
	set_idt_gate(IRQ7, (uint32_t)irq7, 0x08, 0x8E);
	set_idt_gate(IRQ8, (uint32_t)irq8, 0x08, 0x8E);
	set_idt_gate(IRQ9, (uint32_t)irq9, 0x08, 0x8E);
	set_idt_gate(IRQ10, (uint32_t)irq10, 0x08, 0x8E);
	set_idt_gate(IRQ11, (uint32_t)irq11, 0x08, 0x8E);
	set_idt_gate(IRQ12, (uint32_t)irq12, 0x08, 0x8E);
	set_idt_gate(IRQ13, (uint32_t)irq13, 0x08, 0x8E);
	set_idt_gate(IRQ14, (uint32_t)irq14, 0x08, 0x8E);
	set_idt_gate(IRQ15, (uint32_t)irq15, 0x08, 0x8E);

	info("Loading IDT...");
	load_idt((uint32_t)&idtp);
	success("IDT initialized.");
}

void set_idt_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags)
{
	idt_entry_t* entry = &idt_entries[num];
	entry->base_low = base & 0xFFFF;		  // Bottom 16 bits
	entry->base_high = (base >> 16) & 0xFFFF; // Top 16 bits
	entry->sel = sel;
	entry->always0 = 0;
	entry->flags = flags; /*| 0x60; */
}

void register_interrupt_handler(uint32_t index, handler_t handler)
{
	info("Registering interrupt handler %i...", index);
	interrupt_handlers[index] = handler;
}

void unregister_interrupt_handler(uint32_t index)
{
	info("Unregistering interrupt handler %i...", index);
	interrupt_handlers[index] = 0;
}

// Interrupt service routines
void isr_handler(registers_t regs)
{
	dump_registers(&regs);
	uint8_t		isr_no = regs.int_no;
	const char* handler_msg = isr_messages[isr_no];
	if (isr_no == 13) // General protection fault
	{
		error("General Protection Fault. Code: %i", regs.err_code);
		panic("General Protection Fault.");
	}

	handler_t handler = interrupt_handlers[isr_no];
	if (handler)
	{
		debug("Executing %s ISR handler.", handler_msg);
		handler(regs);
	}
	else
	{
		set_terminal_color(VGA_COLOR_LIGHT_RED);
		panic("Invalid handler for ISR%i", isr_no);
	}
}

// Interrupt request
void irq_handler(registers_t regs)
{
	// dump_registers(&regs);
	uint8_t irq_no = regs.int_no;
	// debug("IRQ%i handler called.", irq_no);

	// Get the handler for this interrupt and execute it.
	handler_t handler = interrupt_handlers[irq_no];
	if (handler)
	{
		// debug("Executing handler.");
		handler(regs);
	}
	else
	{
		set_terminal_color(VGA_COLOR_LIGHT_RED);
		panic("Invalid handler for IRQ%i", irq_no);
	}

	// EOI = End of Interrupt
	// debug("Sending EOI...");
	pic_send_eoi(irq_no);
	enable_interrupts();
}

void dump_registers(registers_t* reg)
{
	debug("Dumping registers:");
	debug("\tcr2: %i", reg->cr2);
	debug("\tds: %i", reg->ds);
	debug("\tedi: %i", reg->edi);
	debug("\tesi: %i", reg->esi);
	debug("\tebp: %i", reg->ebp);
	debug("\tesp: %i", reg->esp);
	debug("\tebx: %i", reg->ebx);
	debug("\tedx: %i", reg->edx);
	debug("\tecx: %i", reg->ecx);
	debug("\teax: %i", reg->eax);
	debug("\tint_no: %i", reg->int_no);
	debug("\terr_code: %i", reg->err_code);
}
