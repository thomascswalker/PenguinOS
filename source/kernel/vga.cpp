#include <vga.h>

void VGA::init()
{
	setForeColor(VGA_COLOR_LIGHT_GREY);
	clear();
	enableCursor(0, VGA_HEIGHT - 1);
}

uint16_t VGA::createEntry(char character, uint8_t color)
{
	return (uint16_t)character | (uint16_t)color << 8;
}

uint32_t VGA::getCursorPosition()
{
	return (row * VGA_WIDTH) + column;
}

void VGA::setCursorPosition(int32_t x, int32_t y)
{
	uint16_t pos = y * VGA_WIDTH + x;
	outb(0x3D4, 0x0F);
	outb(0x3D5, (uint8_t)(pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void VGA::updateCursorPosition()
{
	setCursorPosition(column, row);
}

void VGA::enableCursor(uint32_t start, uint32_t end)
{
	outb(0x3D4, 0x0A);
	outb(0x3D5, (inb(0x3D5) & 0xC0) | start);

	outb(0x3D4, 0x0B);
	outb(0x3D5, (inb(0x3D5) & 0xE0) | end);
}

void VGA::disableCursor()
{
	outb(0x3D4, 0x0A);
	outb(0x3D5, 0x20);
}

// Clears all text in the terminal buffer.
void VGA::clear()
{
	for (uint32_t y = 0; y < VGA_HEIGHT; y++)
	{
		for (uint32_t x = 0; x < VGA_WIDTH; x++)
		{
			const uint32_t index = y * VGA_WIDTH + x;
			buffer[index] = TEXT_BLANK;
		}
	}
	setCursorPosition(0, 0);
}

// Sets the current color to write with.
void VGA::setForeColor(uint8_t newColor)
{
	color = newColor;
}

void VGA::setBackColor(uint8_t newColor)
{
	color = color | newColor << 4;
}

// Resets the current color to the default.
void VGA::resetColor()
{
	setForeColor(VGA_COLOR_LIGHT_GREY);
}

// Writes char `c` to the text buffer. This is written
// at the current cursor position.
void VGA::put(char c)
{
	// Handle special characters
	switch (c)
	{
		case '\n': // New lines
			{
				insertNewLine();
				return;
			}
		case '\t': // Tabs
			{
				uint32_t remainder = column % 4;
				column += remainder != 0 ? remainder : 4;
				return;
			}
	}

	// Display standard characters
	const uint32_t pos = getCursorPosition();
	buffer[pos] = createEntry(c, color);
	if (++column == VGA_WIDTH)
	{
		column = 0;
		if (++row == VGA_HEIGHT)
		{
			row = VGA_HEIGHT - 1;
		}
	}
	updateCursorPosition();
}

// Removes the current character from the text buffer.
void VGA::remchar()
{
	const uint32_t pos = getCursorPosition();
	for (uint32_t i = pos - 1; i < VGA_WIDTH * VGA_HEIGHT; i++)
	{
		buffer[i] = buffer[i + 1];
	}
	buffer[VGA_WIDTH * VGA_HEIGHT] = ' ';
	if (--column < 0)
	{
		column = VGA_WIDTH;
		row--;
	}
}

// Insert a new line into the text buffer, scrolling the
// terminal view as needed.
void VGA::insertNewLine()
{
	column = 0;
	if (++row == VGA_HEIGHT)
	{
		row--;
		scroll();
	}
	updateCursorPosition();
}

// Shift the terminal view down one row. This moves all entries
// up one row and clears the last row.
void VGA::scroll()
{
	// Move the lines up
	for (uint32_t i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++)
	{
		buffer[i] = buffer[i + VGA_WIDTH];
	}

	// Clear the last line
	for (uint32_t i = (VGA_HEIGHT - 1) * VGA_WIDTH; i < VGA_HEIGHT * VGA_WIDTH; i++)
	{
		buffer[i] = TEXT_BLANK;
		column = 0;
	}
	row = VGA_HEIGHT - 1;
}
