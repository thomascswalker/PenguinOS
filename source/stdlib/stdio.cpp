#include <stdio.h>
#include <vga.h>

using namespace VGA;

// Prints the specified character array to the terminal view.
void print(const char* str)
{
	for (size_t i = 0; i < strlen(str); i++)
	{
		put(str[i]);
	}
	updateCursorPosition();
}

void println(const char* str)
{
	print(str);
	print("\n");
	updateCursorPosition();
}

/*
Print to the specified stream.

Specifiers:
	- `%c` (char) 		 : `char`
	- `%s` (string) 	 : `const char*`
	- `%d` (integral) 	 : `uint<8|16|32>_t | int<8|16|32>_t`
	- `%x` (hexadecimal) : `uint<8|16|32>_t | int<8|16|32>_t`
*/
uint32_t sprintf(char* stream, const char* format, va_list args)
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

						// Convert integer to string
						itoa((char*)buffer, value, base);

						// Convert all non-integral chars to uppercase
						for (size_t i = 0; i < strlen(buffer); i++)
						{
							if (!isintegral(buffer[i]))
							{
								buffer[i] = toupper(buffer[i]);
							}
						}
						strcpy(stream, (char*)buffer);
						stream += strlen(buffer);
						break;
					}
				default:
					{
						VGA::setForeColor(VGA_COLOR_RED);
						print("Invalid specifier!");
						halt();
					}
			}
		}
		else
		{
			// Set the current position in the stream to this
			// char and increment the stream.
			*stream++ = c;
		}
	}

	// Null-terminate the string.
	*stream = '\0';

	// Return the length of the final formatted string.
	return stream - begin;
}

void printf(const char* format, ...)
{
	VGA::setForeColor(VGA_COLOR_DEFAULT);
	va_list args;
	va_start(args, format);
	char buffer[512];
	sprintf(buffer, format, args);
	print(buffer);
	va_end(args);
}

void debug(const char* format, ...)
{
	VGA::setForeColor(VGA_COLOR_LIGHT_GREY);
	va_list args;
	va_start(args, format);
	print("[DEBUG  ] ");
	char buffer[512];
	sprintf(buffer, format, args);
	println(buffer);
	va_end(args);
	VGA::resetColor();
}

void info(const char* format, ...)
{
	VGA::setForeColor(VGA_COLOR_WHITE);
	va_list args;
	va_start(args, format);
	print("[INFO   ] ");
	char buffer[512];
	sprintf(buffer, format, args);
	println(buffer);
	va_end(args);
	VGA::resetColor();
}

void warning(const char* format, ...)
{
	VGA::setForeColor(VGA_COLOR_YELLOW);
	va_list args;
	va_start(args, format);
	print("[WARNING] ");
	char buffer[512];
	sprintf(buffer, format, args);
	println(buffer);
	va_end(args);
	VGA::resetColor();
}

void error(const char* format, ...)
{
	VGA::setForeColor(VGA_COLOR_LIGHT_RED);
	va_list args;
	va_start(args, format);
	print("[ERROR  ] ");
	char buffer[512];
	sprintf(buffer, format, args);
	println(buffer);
	va_end(args);
	VGA::resetColor();
}

void success(const char* format, ...)
{
	VGA::setForeColor(VGA_COLOR_LIGHT_GREEN);
	va_list args;
	va_start(args, format);
	print("[SUCCESS] ");
	char buffer[512];
	sprintf(buffer, format, args);
	println(buffer);
	va_end(args);
	VGA::resetColor();
}

void panic(const char* format, ...)
{
	VGA::setForeColor(VGA_COLOR_LIGHT_RED);
	va_list args;
	va_start(args, format);
	print("[PANIC  ] ");
	char buffer[512];
	sprintf(buffer, format, args);
	println(buffer);
	println("\nSYSTEM HALTED\n");
	va_end(args);
	halt();
}