#include <cstdio.h>
#include <pic.h>
#include <sys.h>

void PIC::irqMask(uint8_t index)
{
	uint16_t port;
	uint8_t	 mask;

	if (index < 8)
	{
		port = PIC0_DATA;
	}
	else
	{
		port = PIC1_DATA;
		index -= 8;
	}

	mask = inb(port);
	mask |= (1 << index);
	outb(port, mask);
}

void PIC::irqUnmask(uint8_t index)
{
	uint16_t port;
	uint8_t	 mask;

	if (index < 8)
	{
		port = PIC0_DATA;
	}
	else
	{
		port = PIC1_DATA;
		index -= 8;
	}

	mask = inb(port);
	mask &= ~(1 << index);
	outb(port, mask);
}

void PIC::remap()
{
	uint8_t pic0_mask = inb(PIC0_DATA);
	uint8_t pic1_mask = inb(PIC1_DATA);

	outb(PIC0_COMMAND, 0X11);
	outb(PIC1_COMMAND, 0X11);

	outb(PIC0_DATA, 0x20);
	outb(PIC1_DATA, 0x28);

	outb(PIC0_DATA, 0x04);
	outb(PIC1_DATA, 0x02);

	outb(PIC0_DATA, 0x01);
	outb(PIC1_DATA, 0x01);

	outb(PIC0_DATA, pic0_mask);
	outb(PIC1_DATA, pic1_mask);
}

void PIC::sendEOI(uint8_t index)
{
	if (index >= 40)
	{
		outb(PIC1_COMMAND, PIC_EOI);
	}
	outb(PIC0_COMMAND, PIC_EOI);
}
