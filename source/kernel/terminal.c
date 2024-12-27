#include <terminal.h>

/*
Constructs a new `vga_color_t` where the first 4 bytes
are the forecolor and the other 4 bytes are the backcolor.
	| Backcolor | Forecolor |
	7          4|3          0
*/
static uint16_t make_entry_color(vga_color_t forecolor, vga_color_t backcolor)
{
	// Backcolor is shifted 4 bytes to match the correct
	// memory alignment.
	return (uint16_t)(forecolor | backcolor << 4);
}

/*
Returns a new text element with the specified color. Each
text element is 16-bits (little endian).
- The first 8 bits are the character itself.
- The next 4 bits are the forecolor.
- The last 4 bits are the backcolor.

	| Backcolor | Forecolor | Character |
	15        12|11        8|7          0
*/
static uint16_t create_entry(uint8_t character, uint8_t color)
{
	return (uint16_t)character | (uint16_t)color << 8;
}

// Returns the current cursor position in the terminal view.
static uint32_t get_cursor_pos()
{
	return (g_terminal.row * VGA_WIDTH) + g_terminal.column;
}

// Sets the current cursor position in the terminal view to
// `[x,y]`.
static void set_cursor_pos(int8_t x, int8_t y)
{
	uint16_t pos = y * VGA_WIDTH + x;
	outb(0x3D4, 0x0F);
	outb(0x3D5, (uint8_t)(pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

// Enables visual of the cursor.
static void enable_cursor(uint8_t start, uint8_t end)
{
	outb(0x3D4, 0x0A);
	outb(0x3D5, (inb(0x3D5) & 0xC0) | start);

	outb(0x3D4, 0x0B);
	outb(0x3D5, (inb(0x3D5) & 0xE0) | end);
}

// Disables visual of the cursor.
static void disable_cursor()
{
	outb(0x3D4, 0x0A);
	outb(0x3D5, 0x20);
}

// Clears all text in the terminal buffer.
static void clear_terminal()
{
	for (uint32_t y = 0; y < VGA_HEIGHT; y++)
	{
		for (uint32_t x = 0; x < VGA_WIDTH; x++)
		{
			const uint32_t index = y * VGA_WIDTH + x;
			g_terminal.buffer[index] = create_entry(' ', g_terminal.color);
		}
	}
	set_cursor_pos(0, 0);
}

// Initializes the global terminal object, clears the text buffer,
// and enables the cursor.
static void init_terminal()
{
	g_terminal.row = 0;
	g_terminal.column = 0;
	g_terminal.color = make_entry_color(VGA_COLOR_DEFAULT, VGA_COLOR_BLACK);
	g_terminal.buffer = TERMINAL_BUFFER_START;
	clear_terminal();
	enable_cursor(0, VGA_HEIGHT - 1);
}

// Sets the current color to write with.
static void set_terminal_color(uint8_t color)
{
	g_terminal.color = (vga_color_t)color;
}

// Resets the current color to the default.
static void reset_terminal_color()
{
	g_terminal.color = VGA_COLOR_DEFAULT;
}

// Writes char `c` to the text buffer. This is written
// at the current cursor position.
static void put_terminal(char c)
{
	// Handle special characters
	switch (c)
	{
		case '\n': // New lines
			{
				terminal_new_line();
				return;
			}
		case '\t': // Tabs
			{
				uint32_t remainder = g_terminal.column % 4;
				g_terminal.column += remainder != 0 ? remainder : 4;
				return;
			}
	}

	// Display standard characters
	const uint32_t pos = get_cursor_pos();
	g_terminal.buffer[pos] = create_entry(c, g_terminal.color);
	if (++g_terminal.column == VGA_WIDTH)
	{
		g_terminal.column = 0;
		if (++g_terminal.row == VGA_HEIGHT)
		{
			g_terminal.row = 0;
		}
	}
}

// Insert a new line into the text buffer, scrolling the
// terminal view as needed.
static void terminal_new_line()
{
	g_terminal.column = 0;
	if (++g_terminal.row == VGA_HEIGHT)
	{
		g_terminal.row--;
		scroll_terminal();
	}
	set_cursor_pos(g_terminal.column, g_terminal.row);
}

// Shift the terminal view down one row. This moves all entries
// up one row and clears the last row.
static void scroll_terminal()
{
	uint16_t blank = create_entry(' ', VGA_COLOR_DEFAULT);

	// Move the lines up
	for (uint32_t i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++)
	{
		g_terminal.buffer[i] = g_terminal.buffer[i + VGA_WIDTH];
	}

	// Clear the last line
	for (uint32_t i = (VGA_HEIGHT - 1) * VGA_WIDTH; i < VGA_HEIGHT * VGA_WIDTH; i++)
	{
		g_terminal.buffer[i] = blank;
		g_terminal.column = 0;
	}
}

// Writes the specified string to the text buffer.
static void write_terminal(const char* data, size_t size)
{
	for (size_t i = 0; i < size; i++)
	{
		put_terminal(data[i]);
	}
}
