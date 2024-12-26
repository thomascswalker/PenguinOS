#include <isr.c>

#define PIT_NATURAL_FREQ 1193180
#define PIT_DATA0 0x40
#define PIT_DATA1 0x41
#define PIT_DATA2 0x42
#define PIT_COMMAND 0x43

/* This will keep track of how many ticks that the system
 *  has been running for */
size_t ticks = 0;

/* Handles the timer. In this case, it's very simple: We
 *  increment the 'timer_ticks' variable every time the
 *  timer fires. By default, the timer fires 18.222 times
 *  per second. Why 18.222Hz? Some engineer at IBM must've
 *  been smoking something funky */
void timer_handler(registers_t r)
{
	/* Increment our 'tick count' */
	ticks++;
	if (ticks % 10 == 0)
	{
		println("One second has passed.");
	}
}

/* Sets up the system clock by installing the timer handler
 *  into IRQ0 */
void init_timer(uint32_t frequency)
{
	println("Initializing PIT timer...");
	register_interrupt_handler(IRQ0, timer_handler);

	uint32_t divisor;
	if (frequency)
	{
		divisor = PIT_NATURAL_FREQ / frequency;
	}
	else
	{
		divisor = 0;
	}
	outb(PIT_COMMAND, 0x36);

	// Chop freq up into bytes and send to data0 port
	uint8_t low = (uint8_t)(divisor & 0xFF);
	uint8_t high = (uint8_t)((divisor >> 8) & 0xFF);

	outb(PIT_DATA0, low);
	outb(PIT_DATA0, high);

	println("Timer initialized at %ihz", frequency);
}
