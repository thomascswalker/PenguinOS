#include <io.c>
#include <isr.h>

void panic(char* err)
{
	set_cursor_pos(0, 1);
	set_terminal_color(make_entry_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK));
	println("PANIC: %s", err);
	halt();
}