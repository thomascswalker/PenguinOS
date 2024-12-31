#pragma once

#include <stdarg.h>
#include <terminal.c>

// Printing

enum format_spec
{
	FMT_CHAR = 'c',
	FMT_STRING = 's',
	FMT_INT = 'i'
};
typedef enum format_spec format_spec_t;

// Prints the specified character array to the terminal view.
static void print(const char* str)
{
	write_terminal(str, strlen(str));
	set_cursor_pos(g_terminal.column, g_terminal.row);
}

// Prints the specified character array to the terminal view.
static void println(const char* str)
{
	write_terminal(str, strlen(str));
	putchar('\n');
	set_cursor_pos(g_terminal.column, g_terminal.row);
}

/*
Print to the specified stream.

Specifiers:
	- `%c` : `char`
	- `%s` : `const char*`
	- `%i` : `uint<8|16|32>_t | int<8|16|32>_t`
*/
static void sprintf(char* stream, const char* format, va_list args)
{
	char	 c;
	uint32_t i = 0;

	while ((c = format[i]) != 0)
	{
		char buffer[512];
		if (c == '%')
		{
			i++;		   // Skip the %
			c = format[i]; // Get the format specifier
			switch (c)
			{
				case FMT_CHAR: // Characters
					{
						*stream++ = (char)va_arg(args, int);
						break;
					}
				case FMT_STRING: // Strings
					{
						char* text = (char*)va_arg(args, char*);
						strcpy(stream, text);
						break;
					}
				case FMT_INT: // Integers
					{
						int32_t value = va_arg(args, int);
						itoa((char*)buffer, value, 10);
						strcpy(stream, (char*)buffer);
						stream += strlen(buffer);
						break;
					}
				default:
					{
						set_terminal_color(VGA_COLOR_RED);
						print("Invalid specifier!");
						halt();
					}
			}
		}
		else
		{
			*stream++ = c;
		}
		i++;
	}

	*stream = '\0';
}

static void debug(const char* format, ...)
{
	// #ifdef DEBUG
	set_terminal_color(VGA_COLOR_LIGHT_GREY);
	va_list args;
	va_start(args, format);
	print("[DEBUG] ");
	char buffer[512];
	sprintf(buffer, format, args);
	println(buffer);
	va_end(args);
	reset_terminal_color();
	// #endif
}

static void info(const char* format, ...)
{
	set_terminal_color(VGA_COLOR_WHITE);
	va_list args;
	va_start(args, format);
	print("[INFO] ");
	char buffer[512];
	sprintf(buffer, format, args);
	println(buffer);
	va_end(args);
	reset_terminal_color();
}

static void warning(const char* format, ...)
{
	set_terminal_color(VGA_COLOR_YELLOW);
	va_list args;
	va_start(args, format);
	print("[WARNING] ");
	char buffer[512];
	sprintf(buffer, format, args);
	println(buffer);
	va_end(args);
	reset_terminal_color();
}

static void error(const char* format, ...)
{
	set_terminal_color(VGA_COLOR_LIGHT_RED);
	va_list args;
	va_start(args, format);
	print("[ERROR] ");
	char buffer[512];
	sprintf(buffer, format, args);
	println(buffer);
	va_end(args);
	reset_terminal_color();
}

static void success(const char* format, ...)
{
	set_terminal_color(VGA_COLOR_LIGHT_GREEN);
	va_list args;
	va_start(args, format);
	print("[SUCCESS] ");
	char buffer[512];
	sprintf(buffer, format, args);
	println(buffer);
	va_end(args);
	reset_terminal_color();
}

static void panic(const char* format, ...)
{
	set_terminal_color(VGA_COLOR_LIGHT_RED);
	va_list args;
	va_start(args, format);
	print("[PANIC] ");
	char buffer[512];
	sprintf(buffer, format, args);
	println(buffer);
	println("System halted.\n");
	va_end(args);
	halt();
}