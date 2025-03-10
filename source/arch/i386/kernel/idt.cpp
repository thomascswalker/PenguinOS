#include <idt.h>
#include <pic.h>
#include <stdio.h>
#include <syscall.h>

static const char* idtMessages[] = {
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

	IDTEntry entries[IDT_ENTRY_COUNT];
	IDTPtr	 ptr;
	Handler	 handlers[IDT_ENTRY_COUNT];

	void init()
	{
		// Setup PIC
		PIC::remap();

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

	void registerInterruptHandler(uint32_t index, Handler handler) { handlers[index] = handler; }

	void unregisterInterruptHandler(uint32_t index) { handlers[index] = 0; }

	// Interrupt service routines
	void isrHandler(Registers regs)
	{
		switch (regs.intNo)
		{
			case INVALID_OPCODE:
				dumpRegisters(&regs);
				panic("Invalid Opcode.");
				break;
			case DOUBLE_FAULT:
				dumpRegisters(&regs);
				panic("Double Fault. Code: %d", regs.errCode);
				break;
			case GENERAL_PROTECTION_FAULT:
				handleGeneralProtectionFault(&regs);
				break;
			case PAGE_FAULT:
				handlePageFault(&regs);
				break;
			case SYSTEM_CALL:
				// Pass registers to the syscall handler.
				sysCallDispatcher(&regs);
				break;
			default:
				panic("%s exception thrown. Code: %d", idtMessages[regs.intNo], regs.intNo);
				break;
		}
	}

	void handleGeneralProtectionFault(Registers* regs)
	{
		dumpRegisters(regs);

		uint32_t rpl = regs->errCode & 0x3;
		uint32_t ti = (regs->errCode >> 2) & 1;
		uint32_t index = (regs->errCode >> 3);
		panic("General protection fault. Code %x\n"
			  "\t\t  Requested Privilege Level: %d\n"
			  "\t\t  Table Type: %d\n"
			  "\t\t  Table Index: %d\n",
			regs->errCode, rpl, ti, index);
	}

	void handlePageFault(Registers* regs)
	{
		dumpRegisters(regs);

		// Obtain the fault address from the CR2 register.
		uint32_t addr = 0;
		asm("movl %%cr2, %0" : "=r"(addr));

		const char* violationMessage =
			(regs->errCode & 0x1) ? "page-protection" : "non-present page";
		panic("Page fault. Code %x\n"
			  "\t\t  Attempted to access %x which caused a %s violation.",
			regs->errCode, addr, violationMessage);
	}

	// Interrupt request
	void irqHandler(Registers regs)
	{
		uint8_t irq_no = regs.intNo;

		// Get the handler for this interrupt and execute it.
		Handler handler = handlers[irq_no];
		if (handler)
		{
			handler(&regs);
		}

		PIC::sendEOI(irq_no);
	}

	void dumpRegisters(Registers* reg)
	{
		warning("Dumping registers:");
		warning("edi: %x, esi: %x, ebp: %x, esp: %x, ebx: %x, edx: %x, ecx: %x, eax: %x", reg->edi,
			reg->esi, reg->ebp, reg->esp, reg->ebx, reg->edx, reg->ecx, reg->eax);
		warning("int_no: %d, err_code: %d", reg->intNo, reg->errCode);
		warning("Instruction Pointer (eip): %x", reg->eip);

		// Obtain the current frame pointer
		void** frame;
		asm("mov %%ebp, %0" : "=r"(frame));

		// Walk the frame pointer chain.
		while (frame)
		{
			// The return address is stored right above the saved frame pointer.
			void* return_addr = *(frame + 1);
			if (!return_addr)
			{
				break;
			}

			// Print the return address; replace kprintf with your debug printing function.
			printf("Return address: %x\n", return_addr);

			// Move to the previous frame pointer.
			frame = (void**)*frame;
		}
	}
} // namespace IDT
