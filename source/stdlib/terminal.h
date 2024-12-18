#pragma once

#if defined(__linux__)
#error "You are not using a cross-compiler."
#endif

#if !defined(__i386__)
// #error "This needs to be compiled with ix86-elf."
#endif

#include "string.h"
#include "system.h"

// VGA

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

#define VGA_INDEX_MDA 0x3b4
#define VGA_GCR_ADDR 0x3ce
#define VGA_PORT_INDEX_CGA 0x3d4

/* Hardware text mode color constants. */
enum vga_color
{
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
	VGA_COLOR_DEFAULT = VGA_COLOR_WHITE,
};
typedef enum vga_color vga_color_t;

static inline uint8 make_entry_color(vga_color_t fore_color, vga_color_t back_color)
{
	// Backcolor is shifted four bytes to match the correct
	// memory alignment.
	return fore_color | (back_color << 4);
}

/*
Each text element is 16-bits (little endian).
- The first 8 bits are the character itself.
- The next 4 bits are the forecolor.
- The last 4 bits are the backcolor.

	| Backcolor | Forecolor | Character |
	15        12|11        8|7          0
*/
static inline uint16 create_entry(unsigned char character, uint8 color)
{
	return (uint16)character | (uint16)color << 8;
}

// Terminal

struct terminal
{
	uint16*		buffer; // Buffer of all terminal text.
	vga_color_t color;	// The current cursor color.
	uint32		x;		// The current cursor row.
	uint32		y;		// The current cursor column.
};
typedef struct terminal terminal_t;
static terminal_t		g_terminal;

#define TERMINAL_BUFFER_START (uint16*)0xB8000

uint32 get_cursor_pos()
{
	return (g_terminal.x * VGA_WIDTH) + g_terminal.y;
}

void enable_cursor(uint8 start, uint8 end)
{
	outb(0x3D4, 0x0A);
	outb(0x3D5, (inb(0x3D5) & 0xC0) | start);

	outb(0x3D4, 0x0B);
	outb(0x3D5, (inb(0x3D5) & 0xE0) | end);
}

void disable_cursor()
{
	outb(0x3D4, 0x0A);
	outb(0x3D5, 0x20);
}

void set_cursor(int8 x, int8 y)
{
	uint16 pos = y * VGA_WIDTH + x;
	outb(0x3D4, 0x0F);
	outb(0x3D5, (uint8)(pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (uint8)((pos >> 8) & 0xFF));
}

void init_terminal()
{
	g_terminal.x = 0;
	g_terminal.y = 0;
	g_terminal.color = make_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	g_terminal.buffer = TERMINAL_BUFFER_START;
	for (uint32 y = 0; y < VGA_HEIGHT; y++)
	{
		for (uint32 x = 0; x < VGA_WIDTH; x++)
		{
			const uint32 index = y * VGA_WIDTH + x;
			g_terminal.buffer[index] = create_entry(' ', g_terminal.color);
		}
	}
	enable_cursor(0, VGA_HEIGHT - 1);
}

void set_terminal_color(uint8 color)
{
	g_terminal.color = color;
}

void reset_terminal_color()
{
	g_terminal.color = VGA_COLOR_WHITE;
}

void put_terminal(char c)
{
	// Handle special characters
	switch (c)
	{
		case '\n':
			{
				g_terminal.x++;
				g_terminal.y = 0;
				return;
			}
		case '\t':
			{
				uint32 remainder = g_terminal.y % 4;
				g_terminal.y += remainder != 0 ? remainder : 4;
				return;
			}
	}

	// Display standard characters
	const uint32 pos = get_cursor_pos();
	g_terminal.buffer[pos] = create_entry(c, g_terminal.color);
	if (++g_terminal.y == VGA_WIDTH)
	{
		g_terminal.y = 0;
		if (++g_terminal.x == VGA_HEIGHT)
		{
			g_terminal.x = 0;
		}
	}
}

void scroll_terminal()
{
	uint32 blank, temp;

	// Space
	blank = 0x20 | (g_terminal.color << 8);

	if (g_terminal.x >= VGA_HEIGHT)
	{
		temp = g_terminal.x - VGA_HEIGHT + 1;
		memcpy(g_terminal.buffer, g_terminal.buffer + temp * VGA_WIDTH, (VGA_HEIGHT - temp) * VGA_WIDTH * 2);

		memsetw(g_terminal.buffer + (VGA_HEIGHT - temp) * VGA_WIDTH, blank, VGA_WIDTH);
		g_terminal.x = VGA_HEIGHT - 1;
	}
}

void write_terminal(const char* data, size_t size)
{
	for (size_t i = 0; i < size; i++)
	{
		put_terminal(data[i]);
	}
}

// Printing

enum format_spec
{
	FMT_STRING = 's',
	FMT_INT = 'i'
};
typedef enum format_spec format_spec_t;

void print(const char* str)
{
	write_terminal(str, str_length(str));
	set_cursor(g_terminal.y, g_terminal.x);
}

void println(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	size_t i = 0; // Track current position in 'fmt'

	while (i < str_length(fmt))
	{
		char c = fmt[i]; // Get the current character
		if (!is_ascii(c))
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
					print(va_arg(args, char*));
					break;
				}
			case FMT_INT:
				{
					const char* string = itos(va_arg(args, int));
					print(string);
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
	set_cursor(g_terminal.y, g_terminal.x);
}