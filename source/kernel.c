#include "stdlib/terminal.h"

void kernel_main(void)
{
	init_terminal();
	set_terminal_color(VGA_COLOR_LIGHT_RED);
	formatln("Welcome to PengOS.");
	reset_terminal_color();
	formatln("\tThomas Walker");
	formatln("Format line: %d", 5);
}
