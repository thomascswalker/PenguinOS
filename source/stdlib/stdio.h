#pragma once

#include <stdarg.h>
#include <stdlib.h>
#include <vga.h>

// Printing

enum format_spec
{
	FMT_CHAR = 'c',
	FMT_STRING = 's',
	FMT_INT = 'd',
	FMT_HEX = 'x'
};
typedef enum format_spec format_spec_t;

// Prints the specified character array to the terminal view.
static void print(const char* str)
{
	write_terminal(str, strlen(str));
	update_cursor_pos();
}

// Prints the specified character array to the terminal view.
static void println(const char* str)
{
	write_terminal(str, strlen(str));
	putchar('\n');
	update_cursor_pos();
}

/*
Print to the specified stream.

Specifiers:
	- `%c` : `char`
	- `%s` : `const char*`
	- `%d` : `uint<8|16|32>_t | int<8|16|32>_t`
*/
static uint32_t sprintf(char* stream, const char* format, va_list args)
{
	uint8_t		base;
	char		c;
	const char* begin = stream;
	const char* hex_prefix = "0x";

	while ((c = *format++) != '\0')
	{
		char buffer[512];
		if (c == '%')
		{
			c = *format++; // Get the format specifier
			switch (c)
			{
				case FMT_CHAR: // Characters
					{
						*(stream++) = va_arg(args, int);
						break;
					}
				case FMT_STRING: // Strings
					{
						const char* text = va_arg(args, const char*);
						if (!text)
						{
							break;
						}
						while (*text)
						{
							*(stream++) = *(text++);
						}
						break;
					}
				case FMT_INT: // Integers
				case FMT_HEX: // Hexaadecimal
					{
						if (c == 'd')
						{
							base = 10;
						}
						// If hexadecimal, add '0x' to the beginning
						// of the output string.
						else if (c == 'x')
						{
							base = 16;
							strcpy(stream, hex_prefix);
							stream += 2;
						}
						int32_t value = va_arg(args, int);
						itoa((char*)buffer, value, base);
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
	}

	*stream = '\0';
	return stream - begin;
}

static void printf(const char* format, ...)
{
	set_terminal_color(VGA_COLOR_DEFAULT);
	va_list args;
	va_start(args, format);
	char buffer[512];
	sprintf(buffer, format, args);
	print(buffer);
	va_end(args);
}

static void debug(const char* format, ...)
{
	set_terminal_color(VGA_COLOR_LIGHT_GREY);
	va_list args;
	va_start(args, format);
	print("[DEBUG] ");
	char buffer[512];
	sprintf(buffer, format, args);
	println(buffer);
	va_end(args);
	reset_terminal_color();
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