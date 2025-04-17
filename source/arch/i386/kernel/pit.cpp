#include <cstdio.h>
#include <pic.h>
#include <pit.h>
#include <scheduler.h>

// Global ticks for the timer.
static uint32_t g_ticks = 0;

// Initialize `g_sleepTicks` at specific location.
static uint32_t* g_sleepTicks = (uint32_t*)SLEEP_TICK_LOCATION;

void PIT::init()
{
	IDT::registerInterruptHandler(IRQ0, &callback);

	uint16_t divisor = PIT_NATURAL_FREQ / PIT_FREQ;

	outb(PIT_COMMAND, 0x36);

	// Split frequency into high and low bytes and send to
	// data port 0 (64).
	uint8_t low = (uint8_t)(divisor & 0xFF);
	uint8_t high = (uint8_t)((divisor >> 8) & 0xFF);

	outb(PIT_DATA0, low);
	outb(PIT_DATA0, high);

	PIC::sendEOI(IRQ0);
}

void PIT::callback(CPUState* regs)
{
	g_ticks++;
	if (SLEEP_TICK > 0)
	{
		SLEEP_TICK--;
	}
	PIC::sendEOI(regs->intNo);
	Scheduler::yield();
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

uint32_t PIT::getTimerTick() { return g_ticks; }
