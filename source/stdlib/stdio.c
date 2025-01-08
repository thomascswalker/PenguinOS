#include <stdio.h>

void panic(const char* format, ...)
{
	set_terminal_color(VGA_COLOR_LIGHT_RED);
	va_list args;
	va_start(args, format);
	print("[PANIC  ] ");
	char buffer[512];
	sprintf(buffer, format, args);
	println(buffer);
	println("System halted.\n");
	va_end(args);
	halt();
}