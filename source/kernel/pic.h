#pragma once

#include <stdint.h>

#define PIC0_COMMAND 0x20
#define PIC1_COMMAND 0xA0
#define PIC0_DATA (PIC0_COMMAND + 1)
#define PIC1_DATA (PIC1_COMMAND + 1)
#define PIC_EOI 0x20

namespace PIC
{
	void irqMask(uint8_t index);
	void irqUnmask(uint8_t index);
	void remap();
	void sendEOI(uint8_t index);
} // namespace PIC
