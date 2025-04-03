#include <shell.h>
#include <stdio.h>

void putc(char c) { Shell::putNext(c); }

char getc() { return 0; }

// Prints the specified character array to the terminal view.
void print(const char* str)
{
	for (size_t i = 0; i < strlen(str); i++)
	{
		putc(str[i]);
	}
}

void println(const char* str)
{
	print(str);
	print("\n");
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
	const char* hexPrefix = "0x";

	while ((c = *format++) != '\0')
	{
		char buffer[512];
		if (c == '%')
		{
			c = *format++; // Get the format specifier

			bool	 leftAlign = false;
			bool	 zeroPad = false;
			uint32_t width = 0;

			if (c == '-')
			{
				leftAlign = true;
				c = *format++;
			}
			if (c == '0')
			{
				zeroPad = true;
				c = *format++;
			}

			while (c >= '0' && c <= '9')
			{
				width = width * 10 + (c - '0');
				c = *format++;
			}

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

						size_t len = strlen(text);
						int	   padding = width > len ? width - len : 0;

						// Apply padding for right alignment (default)
						if (!leftAlign)
						{
							while (padding-- > 0)
							{
								*(stream++) = zeroPad ? '0' : ' ';
							}
						}

						while (*text)
						{
							*(stream++) = *(text++);
						}

						if (leftAlign)
						{
							while (padding-- > 0)
							{
								*(stream++) = ' ';
							}
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
							strcpy(stream, hexPrefix);
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

						size_t	 len = strlen(buffer);
						uint32_t padding = width > len ? width - len : 0;

						if (!leftAlign)
						{
							// Add padding to the left
							for (uint32_t i = 0; i < padding; i++)
							{
								*(stream++) = zeroPad ? '0' : ' ';
							}
						}

						// Copy the number
						strcpy(stream, buffer);
						stream += len;

						// Add padding for left alignment
						if (leftAlign)
						{
							while (padding-- > 0)
							{
								*(stream++) = ' ';
							}
						}

						// strcpy(stream, (char*)buffer);
						// stream += strlen(buffer);
						break;
					}
				default:
					{
						Shell::setForeColor(VGA_COLOR_RED);
						print("Invalid specifier!");
						syshalt();
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
	va_list args;
	va_start(args, format);
	char buffer[512];
	sprintf(buffer, format, args);
	print(buffer);
	va_end(args);
}

void debug(const char* format, ...)
{
	Shell::setForeColor(VGA_COLOR_LIGHT_GREY);
	va_list args;
	va_start(args, format);
	print("[DEBUG  ] ");
	char buffer[512];
	sprintf(buffer, format, args);
	println(buffer);
	va_end(args);
	Shell::resetColor();
}

void info(const char* format, ...)
{
	Shell::setForeColor(VGA_COLOR_WHITE);
	va_list args;
	va_start(args, format);
	print("[INFO   ] ");
	char buffer[512];
	sprintf(buffer, format, args);
	println(buffer);
	va_end(args);
	Shell::resetColor();
}

void warning(const char* format, ...)
{
	Shell::setForeColor(VGA_COLOR_YELLOW);
	va_list args;
	va_start(args, format);
	print("[WARNING] ");
	char buffer[512];
	sprintf(buffer, format, args);
	println(buffer);
	va_end(args);
	Shell::resetColor();
}

void error(const char* format, ...)
{
	Shell::setForeColor(VGA_COLOR_LIGHT_RED);
	va_list args;
	va_start(args, format);
	print("[ERROR  ] ");
	char buffer[512];
	sprintf(buffer, format, args);
	println(buffer);
	va_end(args);
	Shell::resetColor();
}

void success(const char* format, ...)
{
	Shell::setForeColor(VGA_COLOR_LIGHT_GREEN);
	va_list args;
	va_start(args, format);
	print("[SUCCESS] ");
	char buffer[512];
	sprintf(buffer, format, args);
	println(buffer);
	va_end(args);
	Shell::resetColor();
}

void panic(const char* format, ...)
{
	Shell::setForeColor(VGA_COLOR_LIGHT_RED);
	va_list args;
	va_start(args, format);
	print("[PANIC  ] ");
	char buffer[512];
	sprintf(buffer, format, args);
	println(buffer);
	println("\nSYSTEM HALTED\n");
	va_end(args);
	syshalt();
}