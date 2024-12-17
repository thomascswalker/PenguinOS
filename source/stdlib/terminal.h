#pragma once

#if defined(__linux__)
#error "You are not using a cross-compiler."
#endif

#if !defined(__i386__)
// #error "This needs to be compiled with ix86-elf."
#endif

#include "string.h"

// VGA

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

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
	return fore_color | back_color << 4;
}

static inline uint16 create_entry(unsigned char character, uint8 color)
{
	return (uint16)character | (uint16)color << 8;
}

// Terminal

struct terminal
{
	uint16*		buffer; // Buffer of all terminal text.
	vga_color_t color;	// The current cursor color.
	uint32		row;	// The current cursor row.
	uint32		column; // The current cursor column.
};
typedef struct terminal terminal_t;
static terminal_t		g_terminal;

#define TERMINAL_BUFFER_START (uint16*)0xB8000

void init_terminal(void)
{
	g_terminal.row = 0;
	g_terminal.column = 0;
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
}

void set_terminal_color(uint8 color)
{
	g_terminal.color = color;
}

void put_terminal(char c)
{
	// Handle special characters
	switch (c)
	{
		case '\n':
			{
				g_terminal.row++;
				g_terminal.column = 0;
				return;
			}
		case '\t':
			{
				uint32 remainder = g_terminal.column % 4;
				g_terminal.column += remainder != 0 ? remainder : 4;
				return;
			}
	}

	// Display standard characters
	const uint32 index = g_terminal.row * VGA_WIDTH + g_terminal.column;
	g_terminal.buffer[index] = create_entry(c, g_terminal.color);
	if (++g_terminal.column == VGA_WIDTH)
	{
		g_terminal.column = 0;
		if (++g_terminal.row == VGA_HEIGHT)
		{
			g_terminal.row = 0;
		}
	}
}

void write_terminal(const char* data, size_t size)
{
	for (size_t i = 0; i < size; i++)
	{
		put_terminal(data[i]);
	}
}

void println(const char* data)
{
	write_terminal(data, strlen(data));
}

void printlnc(const char* data, vga_color_t color)
{
	// Store the current color as previous.
	vga_color_t prev_color = g_terminal.color;
	// Temporarily set the terminal color to the specified color.
	set_terminal_color(color);
	// Write the text to the terminal.
	write_terminal(data, strlen(data));
	// Reset the terminal color back to the previous color.
	set_terminal_color(prev_color);
}
