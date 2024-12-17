#include "stdlib/terminal.h"

void kernel_main(void)
{
	init_terminal();
	set_terminal_color(VGA_COLOR_LIGHT_RED);
	println("Welcome to PengOS.");
	reset_terminal_color();
	println("\tThomas Walker");
	println("I have %i %s.", 12345, "apples");
}
