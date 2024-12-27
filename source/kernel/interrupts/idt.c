#include <idt.h>
#include <io.c>

idt_entry_t idt_entries[IDT_ENTRY_COUNT];
idt_ptr_t	idtp;

extern void load_idt(uint32_t);

void init_idt()
{
	println("Initializing IDT...");

	idtp.size = sizeof(idt_entry_t) * IDT_ENTRY_COUNT - 1;
	idtp.addr = (uint32_t)&idt_entries;

	memset(&idt_entries, 0, sizeof(idt_entry_t) * IDT_ENTRY_COUNT);

	// Setup PIC
	println("Initializing PIC...");

	outb(PIC0_COMMAND, 0x11);
	outb(PIC1_COMMAND, 0x11);

	outb(PIC0_DATA, 0x20); // Vector offsets
	outb(PIC1_DATA, 0x28);

	outb(PIC0_DATA, 0x04);
	outb(PIC1_DATA, 0x02);

	outb(PIC0_DATA, 0x01);
	outb(PIC1_DATA, 0x01);

	outb(PIC0_DATA, 0x00);
	outb(PIC1_DATA, 0x00);

	println("PIC initialized.");

	println("Setting ISR gates...");
	set_idt_gate(0, (uint32_t)isr0, 0x08, 0x8E);
	set_idt_gate(1, (uint32_t)isr1, 0x08, 0x8E);
	set_idt_gate(2, (uint32_t)isr2, 0x08, 0x8E);
	set_idt_gate(3, (uint32_t)isr3, 0x08, 0x8E);
	set_idt_gate(4, (uint32_t)isr4, 0x08, 0x8E);
	set_idt_gate(5, (uint32_t)isr5, 0x08, 0x8E);
	set_idt_gate(6, (uint32_t)isr6, 0x08, 0x8E);
	set_idt_gate(7, (uint32_t)isr7, 0x08, 0x8E);
	set_idt_gate(8, (uint32_t)isr8, 0x08, 0x8E);
	set_idt_gate(9, (uint32_t)isr9, 0x08, 0x8E);
	set_idt_gate(10, (uint32_t)isr10, 0x08, 0x8E);
	set_idt_gate(11, (uint32_t)isr11, 0x08, 0x8E);
	set_idt_gate(12, (uint32_t)isr12, 0x08, 0x8E);
	set_idt_gate(13, (uint32_t)isr13, 0x08, 0x8E);
	set_idt_gate(14, (uint32_t)isr14, 0x08, 0x8E);
	set_idt_gate(15, (uint32_t)isr15, 0x08, 0x8E);
	set_idt_gate(16, (uint32_t)isr16, 0x08, 0x8E);
	set_idt_gate(17, (uint32_t)isr17, 0x08, 0x8E);
	set_idt_gate(18, (uint32_t)isr18, 0x08, 0x8E);
	set_idt_gate(19, (uint32_t)isr19, 0x08, 0x8E);
	set_idt_gate(20, (uint32_t)isr20, 0x08, 0x8E);
	set_idt_gate(21, (uint32_t)isr21, 0x08, 0x8E);
	set_idt_gate(22, (uint32_t)isr22, 0x08, 0x8E);
	set_idt_gate(23, (uint32_t)isr23, 0x08, 0x8E);
	set_idt_gate(24, (uint32_t)isr24, 0x08, 0x8E);
	set_idt_gate(25, (uint32_t)isr25, 0x08, 0x8E);
	set_idt_gate(26, (uint32_t)isr26, 0x08, 0x8E);
	set_idt_gate(27, (uint32_t)isr27, 0x08, 0x8E);
	set_idt_gate(28, (uint32_t)isr28, 0x08, 0x8E);
	set_idt_gate(29, (uint32_t)isr29, 0x08, 0x8E);
	set_idt_gate(30, (uint32_t)isr30, 0x08, 0x8E);
	set_idt_gate(31, (uint32_t)isr31, 0x08, 0x8E);

	println("Setting IRQ gates...");
	set_idt_gate(32, (uint32_t)irq0, 0x08, 0x8E);
	set_idt_gate(33, (uint32_t)irq1, 0x08, 0x8E);
	set_idt_gate(34, (uint32_t)irq2, 0x08, 0x8E);
	set_idt_gate(35, (uint32_t)irq3, 0x08, 0x8E);
	set_idt_gate(36, (uint32_t)irq4, 0x08, 0x8E);
	set_idt_gate(37, (uint32_t)irq5, 0x08, 0x8E);
	set_idt_gate(38, (uint32_t)irq6, 0x08, 0x8E);
	set_idt_gate(39, (uint32_t)irq7, 0x08, 0x8E);
	set_idt_gate(40, (uint32_t)irq8, 0x08, 0x8E);
	set_idt_gate(41, (uint32_t)irq9, 0x08, 0x8E);
	set_idt_gate(42, (uint32_t)irq10, 0x08, 0x8E);
	set_idt_gate(43, (uint32_t)irq11, 0x08, 0x8E);
	set_idt_gate(44, (uint32_t)irq12, 0x08, 0x8E);
	set_idt_gate(45, (uint32_t)irq13, 0x08, 0x8E);
	set_idt_gate(46, (uint32_t)irq14, 0x08, 0x8E);
	set_idt_gate(47, (uint32_t)irq15, 0x08, 0x8E);

	println("Setting system call gates...");
	set_idt_gate(128, (uint32_t)isr128, 0x08, 0x8E); // System calls
	set_idt_gate(177, (uint32_t)isr177, 0x08, 0x8E); // System calls

	println("Loading IDT...");
	load_idt((uint32_t)&idtp);
	println("IDT initialized.");
}

void set_idt_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags)
{
	idt_entry_t* entry = &idt_entries[num];
	entry->base_low = base & 0xFFFF;
	entry->base_high = (base >> 16) & 0xFFFF;
	entry->sel = sel;
	entry->always0 = 0;
	entry->flags = flags; /*| 0x60; */
}

const char* exception_messages[] = {
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
	"Unknown",
	"x87 floating point exception",
	"Alignment check",
	"Machine check",
	"SIMD floating point exception",
	"Virtualization exception",
	"Control protection exception",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
};

// Interrupt service routines
void isr_handler(registers_t regs)
{
	if (regs.int_no < 32)
	{
		const char* msg = exception_messages[regs.int_no];
		println("%s exception thrown.", msg);
		while (true)
		{
			halt();
		}
	}
}

void irq_install_handler(uint32_t irq, handler_t handler)
{
	println("Installing IRQ%i handler.", irq);
	irq_routines[irq] = handler;
}

void irq_uninstall_handler(uint32_t irq)
{
	println("Uninstalling IRQ%i handler.", irq);
	irq_routines[irq] = 0;
}

// Interrupt request
void irq_handler(registers_t regs)
{
	println("IRQ handler called!");
	handler_t handler;
	handler = irq_routines[regs.int_no - 32];
	if (handler)
	{
		handler(regs);
	}

	if (regs.int_no >= 40)
	{
		outb(0xA0, 0x20);
	}
	outb(0x20, 0x20);
}