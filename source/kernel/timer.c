#include <timer.h>

static uint64_t ticks = 0;
static uint64_t seconds = 0;

void init_timer()
{
	info("Initializing timer...");
	register_interrupt_handler(IRQ0, &timer_callback);

	uint16_t divisor = PIT_NATURAL_FREQ / PIT_FREQ;
	debug("Timer divisor: %i", divisor);

	debug("Outputting command %i at %i.", 0x36, PIT_COMMAND);
	outb(PIT_COMMAND, 0x36);

	// Split frequency into high and low bytes and send to
	// data port 0 (64).
	uint8_t low = (uint8_t)(divisor & 0xFF);
	uint8_t high = (uint8_t)((divisor >> 8) & 0xFF);

	debug("Outputting low value %i at %i.", low, PIT_DATA0);
	outb(PIT_DATA0, low);
	debug("Outputting high value %i at %i.", high, PIT_DATA0);
	outb(PIT_DATA0, high);

	success("Timer initialized.");
}

void timer_callback(registers_t regs)
{
	ticks++;
	if ((ticks % 18) == 0)
	{
		seconds++;
	}
}

uint32_t read_pit_count()
{
	uint32_t count = 0;

	// Disable interrupts
	disable_interrupts();

	// al = channel in bits 6 and 7, remaining bits clear
	outb(0x43, 0b0000000);

	count = inb(0x40);		 // Low byte
	count |= inb(0x40) << 8; // High byte

	enable_interrupts();

	return count;
}
