#pragma once

#include <idt.h>

#define PIT_FREQ 100
#define PIT_NATURAL_FREQ 1193180

#define PIT_DATA0 0x40	 // Channel 0 data port (read/write)
#define PIT_DATA1 0x41	 // Channel 1 data port (read/write)
#define PIT_DATA2 0x42	 // Channel 2 data port (read/write)
#define PIT_COMMAND 0x43 // Mode/Command register (write only, a read is ignored)

void	 init_timer();
void	 timer_callback(registers_t regs);
uint32_t read_pit_count();