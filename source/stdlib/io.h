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

void print(const char* str)
{
	write_terminal(str, strlen(str));
	set_cursor_pos(g_terminal.column, g_terminal.row);
}

void println(const char* fmt, ...)
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
		format_spec_t specifier = fmt[i];

		switch (specifier)
		{
			case FMT_STRING: // Strings
				{
					// print("Formatting string...\n");
					const char* s = va_arg(args, char*);
					const int	l = strlen(s);
					for (int x = 0; x < l; x++)
					{
						put_terminal(s[x]);
					}
					// print(s);
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
