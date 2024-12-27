#pragma once

#include <stdarg.h>
#include <terminal.c>

// Printing

enum format_spec
{
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

/*
Prints the specified character array and formats any arguments
to the terminal view. Once printed, adds a new line.

Specifiers:
	- `%s` : `const char*`
	- `%i` : `uint<8|16|32>_t | int<8|16|32>_t`
*/
static void println(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	size_t i = 0; // Track current position in 'fmt'

	while (i < strlen(fmt))
	{
		char c = fmt[i]; // Get the current character
		if (!isascii(c))
		{
			char* msg = "Invalid token [%].";
			//------------------------- ^ INDEX 16
			msg[16] = c;
			print(msg);
			return;
		}

		// If the current character is not the start of
		// a format token, display it and continue.
		if (c != '%')
		{
			put_terminal(c);
			i++;
			continue;
		}

		// Go to next char (%x, where x is the next)
		i++;
		// Get the format specifier.
		format_spec_t specifier = (format_spec_t)fmt[i];

		switch (specifier)
		{
			case FMT_STRING: // Strings
				{
					const char* s = va_arg(args, char*);
					print(s);
					break;
				}
			case FMT_INT: // Integers
				{
					char	buffer[16];
					int32_t v = va_arg(args, int);
					itos(v, (char*)buffer);
					print(buffer);
					break;
				}
			default:
				print("Invalid specifier.");
				break;
		}
		i++; // Go to next token after the format specifier
	}

	va_end(args);

	put_terminal('\n');
	set_cursor_pos(g_terminal.column, g_terminal.row);
}
