#include <pic.h>
#include <stdio.h>
#include <timer.h>

void init_timer()
{
	info("Initializing timer...");
	register_interrupt_handler(IRQ0, &timer_callback);

	uint16_t divisor = PIT_NATURAL_FREQ / PIT_FREQ;
	debug("Timer divisor: %d", divisor);

	debug("Outputting command %d at %x.", 0x36, PIT_COMMAND);
	outb(PIT_COMMAND, 0x36);

	// Split frequency into high and low bytes and send to
	// data port 0 (64).
	uint8_t low = (uint8_t)(divisor & 0xFF);
	uint8_t high = (uint8_t)((divisor >> 8) & 0xFF);

	debug("Outputting low value %d at %x.", low, PIT_DATA0);
	outb(PIT_DATA0, low);
	debug("Outputting high value %d at %x.", high, PIT_DATA0);
	outb(PIT_DATA0, high);

	pic_send_eoi(IRQ0);

	success("Timer initialized.");
}

void timer_callback(registers_t regs)
{
	g_ticks++;
	if (SLEEP_TICK > 0)
	{
		SLEEP_TICK--;
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
