#include <idt.h>
#include <pic.h>
#include <stdio.h>
#include <syscall.h>

namespace IDT
{

#define PAGE_FAULT_MESSAGE  \
	"Page fault. Code %d\n" \
	"\t\t  Attempted to access %x which is in a non-paged area."

	IDTEntry entries[IDT_ENTRY_COUNT];
	IDTPtr	 ptr;
	Handler	 handlers[IDT_ENTRY_COUNT];

	void init()
	{
		// Setup PIC
		pic_remap();

		ptr.limit = (sizeof(IDTEntry) * IDT_ENTRY_COUNT) - 1;
		ptr.base = (uint32_t)&entries;

		memset(&entries, 0, sizeof(IDTEntry) * IDT_ENTRY_COUNT);

		setGate(ISR0, (uint32_t)isr0, 0x08, 0x8E);
		setGate(ISR1, (uint32_t)isr1, 0x08, 0x8E);
		setGate(ISR2, (uint32_t)isr2, 0x08, 0x8E);
		setGate(ISR3, (uint32_t)isr3, 0x08, 0x8E);
		setGate(ISR4, (uint32_t)isr4, 0x08, 0x8E);
		setGate(ISR5, (uint32_t)isr5, 0x08, 0x8E);
		setGate(ISR6, (uint32_t)isr6, 0x08, 0x8E);
		setGate(ISR7, (uint32_t)isr7, 0x08, 0x8E);
		setGate(ISR8, (uint32_t)isr8, 0x08, 0x8E);
		setGate(ISR9, (uint32_t)isr9, 0x08, 0x8E);
		setGate(ISR10, (uint32_t)isr10, 0x08, 0x8E);
		setGate(ISR11, (uint32_t)isr11, 0x08, 0x8E);
		setGate(ISR12, (uint32_t)isr12, 0x08, 0x8E);
		setGate(ISR13, (uint32_t)isr13, 0x08, 0x8E);
		setGate(ISR14, (uint32_t)isr14, 0x08, 0x8E);
		setGate(ISR15, (uint32_t)isr15, 0x08, 0x8E);
		setGate(ISR16, (uint32_t)isr16, 0x08, 0x8E);
		setGate(ISR17, (uint32_t)isr17, 0x08, 0x8E);
		setGate(ISR18, (uint32_t)isr18, 0x08, 0x8E);
		setGate(ISR19, (uint32_t)isr19, 0x08, 0x8E);
		setGate(ISR20, (uint32_t)isr20, 0x08, 0x8E);
		setGate(ISR21, (uint32_t)isr21, 0x08, 0x8E);
		setGate(ISR22, (uint32_t)isr22, 0x08, 0x8E);
		setGate(ISR23, (uint32_t)isr23, 0x08, 0x8E);
		setGate(ISR24, (uint32_t)isr24, 0x08, 0x8E);
		setGate(ISR25, (uint32_t)isr25, 0x08, 0x8E);
		setGate(ISR26, (uint32_t)isr26, 0x08, 0x8E);
		setGate(ISR27, (uint32_t)isr27, 0x08, 0x8E);
		setGate(ISR28, (uint32_t)isr28, 0x08, 0x8E);
		setGate(ISR29, (uint32_t)isr29, 0x08, 0x8E);
		setGate(ISR30, (uint32_t)isr30, 0x08, 0x8E);
		setGate(ISR31, (uint32_t)isr31, 0x08, 0x8E);

		setGate(ISR128, (uint32_t)isr128, 0x08, 0x8E);

		setGate(IRQ0, (uint32_t)irq0, 0x08, 0x8E);
		setGate(IRQ1, (uint32_t)irq1, 0x08, 0x8E);
		setGate(IRQ2, (uint32_t)irq2, 0x08, 0x8E);
		setGate(IRQ3, (uint32_t)irq3, 0x08, 0x8E);
		setGate(IRQ4, (uint32_t)irq4, 0x08, 0x8E);
		setGate(IRQ5, (uint32_t)irq5, 0x08, 0x8E);
		setGate(IRQ6, (uint32_t)irq6, 0x08, 0x8E);
		setGate(IRQ7, (uint32_t)irq7, 0x08, 0x8E);
		setGate(IRQ8, (uint32_t)irq8, 0x08, 0x8E);
		setGate(IRQ9, (uint32_t)irq9, 0x08, 0x8E);
		setGate(IRQ10, (uint32_t)irq10, 0x08, 0x8E);
		setGate(IRQ11, (uint32_t)irq11, 0x08, 0x8E);
		setGate(IRQ12, (uint32_t)irq12, 0x08, 0x8E);
		setGate(IRQ13, (uint32_t)irq13, 0x08, 0x8E);
		setGate(IRQ14, (uint32_t)irq14, 0x08, 0x8E);
		setGate(IRQ15, (uint32_t)irq15, 0x08, 0x8E);

		loadIDT((uint32_t)&ptr);
		success("IDT initialized.");
	}

	void setGate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags)
	{
		IDTEntry* entry = &entries[num];
		entry->baseLow = base & 0xFFFF;			 // Bottom 16 bits
		entry->baseHigh = (base >> 16) & 0xFFFF; // Top 16 bits
		entry->sel = sel;
		entry->always0 = 0;
		entry->flags = flags | 0x60;
	}

	void registerInterruptHandler(uint32_t index, Handler handler)
	{
		handlers[index] = handler;
	}

	void unregisterInterruptHandler(uint32_t index)
	{
		handlers[index] = 0;
	}

	// Interrupt service routines
	void isrHandler(Registers regs)
	{

		uint8_t isr_no = regs.int_no;
		switch (isr_no)
		{
			case DOUBLE_FAULT:
				panic("Double Fault. Code: %d", regs.err_code);
				break;
			case GENERAL_PROTECTION_FAULT:
				panic("General Protection Fault. Code: %d", regs.err_code);
				break;
			case PAGE_FAULT:
				// Obtain the fault address from the CR2 register.
				uint32_t addr;
				asm("movl %%cr2, %0" : "=r"(addr));
				panic(PAGE_FAULT_MESSAGE, regs.err_code, addr);
				break;
			case SYSTEM_CALL:
				{
					// Pass registers to the syscall handler.
					sysCallDispatcher(regs);
					return;
				}
			default:
				panic("%s exception thrown. Code: %d", idt_messages[regs.int_no], regs.int_no);
				break;
		}
	}

	// Interrupt request
	void irqHandler(Registers regs)
	{
		uint8_t irq_no = regs.int_no;

		// Get the handler for this interrupt and execute it.
		Handler handler = handlers[irq_no];
		if (handler)
		{
			handler(regs);
		}
		// else
		// {
		// 	// dumpRegisters(&regs);
		// 	panic("IRQ handler %d not found!", (irq_no - 32));
		// }

		pic_send_eoi(irq_no);
	}

	void dumpRegisters(Registers* reg)
	{
		warning("Dumping registers:");
		warning("edi: %d, esi: %d, ebp: %d, esp: %d, ebx: %d, edx: %d, ecx: %d, eax: %d", reg->edi,
			reg->esi, reg->ebp, reg->esp, reg->ebx, reg->edx, reg->ecx, reg->eax);
		warning("int_no: %d, err_code: %d", reg->int_no, reg->err_code);
	}
} // namespace IDT
