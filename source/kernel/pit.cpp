#include <pic.h>
#include <pit.h>
#include <stdio.h>

static uint64_t ticks = 0;
static uint64_t seconds = 0;

void PIT::init()
{
	info("Initializing timer...");
	IDT::registerInterruptHandler(IRQ0, &callback);

	uint16_t divisor = PIT_NATURAL_FREQ / PIT_FREQ;

	outb(PIT_COMMAND, 0x36);

	// Split frequency into high and low bytes and send to
	// data port 0 (64).
	uint8_t low = (uint8_t)(divisor & 0xFF);
	uint8_t high = (uint8_t)((divisor >> 8) & 0xFF);

	outb(PIT_DATA0, low);
	outb(PIT_DATA0, high);

	pic_send_eoi(IRQ0);

	success("Timer initialized.");
}

void PIT::callback(Registers regs)
{
	ticks++;
	if ((ticks % PIT_FREQ) == 0)
	{
		seconds++;
	}
}

uint32_t PIT::getPITCount()
{
	uint32_t count = 0;

	// Disable interrupts
	disableInterrupts();

	// al = channel in bits 6 and 7, remaining bits clear
	outb(0x43, 0b0000000);

	count = inb(0x40);		 // Low byte
	count |= inb(0x40) << 8; // High byte

	enableInterrupts();

	return count;
}
