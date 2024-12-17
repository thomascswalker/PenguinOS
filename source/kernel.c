#include "stdlib/terminal.h"

void kernel_main(void)
{
	init_terminal();
	set_terminal_color(VGA_COLOR_GREEN);
	println("Welcome to PengOS.\n");
	set_terminal_color(VGA_COLOR_DEFAULT);
	println("\tThomas Walker\n");
	println("\tmeow");
}
