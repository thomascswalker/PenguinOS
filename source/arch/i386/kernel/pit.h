#pragma once

#include <idt.h>

#define PIT_FREQ 1000
#define PIT_NATURAL_FREQ 1193180

#define PIT_DATA0 0x40	 // Channel 0 data port (read/write)
#define PIT_DATA1 0x41	 // Channel 1 data port (read/write)
#define PIT_DATA2 0x42	 // Channel 2 data port (read/write)
#define PIT_COMMAND 0x43 // Mode/Command register (write only, a read is ignored)

// TODO: Resolve this mess. Why do we need a pointer to the
// sleep tick? Can't we just use a normal static int?
// Sleep tick memory is stored at a specific location.
#define SLEEP_TICK_LOCATION (0x1700)
#define SLEEP_TICK_PTR ((uint32_t*)SLEEP_TICK_LOCATION) // Pointer to `g_sleep_ticks`.
#define SLEEP_TICK (*SLEEP_TICK_PTR)					// Value of `g_sleep_ticks`.

namespace PIT
{
	void	 init();
	void	 callback(Registers* regs);
	uint32_t getPITCount();
} // namespace PIT