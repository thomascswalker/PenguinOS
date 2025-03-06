#include <cmd.h>
#include <shell.h>
#include <stdio.h>
#include <string.h>

static uint16_t* g_displayBuffer;
static uint16_t* g_inputBuffer;

static size_t g_inputCursor;

static int32_t g_displayRow;
static int32_t g_displayColumn;
static uint8_t g_displayColor;

void Shell::init()
{
	g_displayBuffer = VGA_BUFFER_START;
	g_inputBuffer = g_displayBuffer + (VGA_WIDTH * 24) + (strlen(INPUT_PROMPT));
	setForeColor(VGA_COLOR_LIGHT_GREY);
	clearDisplay();
	enableCursor(0, VGA_HEIGHT - 1);

	for (uint32_t x = 0; x < VGA_WIDTH; x++)
	{
		uint32_t offset = ((VGA_HEIGHT - 2) * VGA_WIDTH) + x;
		g_displayBuffer[offset] = createEntry('-', g_displayColor);
	}

	const char* prompt = INPUT_PROMPT;
	for (uint32_t x = 0; x < strlen(prompt); x++)
	{
		uint32_t offset = ((VGA_HEIGHT - 1) * VGA_WIDTH) + x;
		g_displayBuffer[offset] = createEntry(prompt[x], g_displayColor);
	}

	setCursorPosition(0);
}

uint16_t Shell::createEntry(char character, uint8_t color)
{
	return (uint16_t)character | (uint16_t)color << 8;
}

uint32_t Shell::getDisplayPosition() { return (g_displayRow * VGA_WIDTH) + g_displayColumn; }

void Shell::setDisplayPosition(uint32_t x, uint32_t y)
{
	g_displayRow = y;
	g_displayColumn = x;
}

void Shell::setCursorPosition(int32_t x)
{
	uint16_t pos = INPUT_OFFSET + x;
	outb(0x3D4, 0x0F);
	outb(0x3D5, (uint8_t)(pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void Shell::updateCursorPosition() { setCursorPosition(g_inputCursor); }

void Shell::enableCursor(uint32_t start, uint32_t end)
{
	outb(0x3D4, 0x0A);
	outb(0x3D5, (inb(0x3D5) & 0xC0) | start);

	outb(0x3D4, 0x0B);
	outb(0x3D5, (inb(0x3D5) & 0xE0) | end);
}

void Shell::disableCursor()
{
	outb(0x3D4, 0x0A);
	outb(0x3D5, 0x20);
}

// Clears all text in the terminal buffer.
void Shell::clearDisplay()
{
	for (uint32_t y = 0; y < DISPLAY_HEIGHT; y++)
	{
		for (uint32_t x = 0; x < VGA_WIDTH; x++)
		{
			const uint32_t index = y * VGA_WIDTH + x;
			g_displayBuffer[index] = TEXT_BLANK;
		}
	}
	g_displayRow = 0;
	g_displayColumn = 0;
}

void Shell::clearInput()
{
	wmemset(g_inputBuffer, TEXT_BLANK, VGA_WIDTH * (sizeof(uint16_t)));
	g_inputCursor = 0;
	setCursorPosition(0);
}

// Sets the current color to write with.
void Shell::setForeColor(uint8_t newColor) { g_displayColor = newColor; }

void Shell::setBackColor(uint8_t newColor) { g_displayColor = g_displayColor | newColor << 4; }

// Resets the current color to the default.
void Shell::resetColor() { setForeColor(VGA_COLOR_LIGHT_GREY); }

// Writes char `c` to the text buffer. This is written
// at the current cursor position.
void Shell::putNext(char c)
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
				uint32_t remainder = g_displayColumn % 4;
				g_displayColumn += remainder != 0 ? remainder : 4;
				return;
			}
	}

	// Display standard characters
	const uint32_t pos = getDisplayPosition();
	g_displayBuffer[pos] = createEntry(c, g_displayColor);
	if (++g_displayColumn == VGA_WIDTH)
	{
		g_displayColumn = 0;
		if (++g_displayRow == DISPLAY_HEIGHT)
		{
			scroll();
		}
	}
}

void Shell::putAt(char c, uint32_t pos) { g_displayBuffer[pos] = createEntry(c, g_displayColor); }

// Insert a new line into the text buffer, scrolling the
// terminal view as needed.
void Shell::insertNewLine()
{
	g_displayColumn = 0;
	if (++g_displayRow >= DISPLAY_HEIGHT)
	{
		scroll();
	}
}

// Shift the terminal view down one row. This moves all entries
// up one row and clears the last row.
void Shell::scroll()
{
	// Move the lines up
	for (size_t row = 1; row < DISPLAY_HEIGHT; row++)
	{
		for (size_t column = 0; column < VGA_WIDTH; column++)
		{
			g_displayBuffer[(row - 1) * VGA_WIDTH + column] =
				g_displayBuffer[row * VGA_WIDTH + column];
		}
	}
	for (size_t column = 0; column < VGA_WIDTH; column++)
	{
		g_displayBuffer[(DISPLAY_HEIGHT - 1) * VGA_WIDTH + column] = TEXT_BLANK;
	}
	g_displayRow = DISPLAY_HEIGHT - 1;
}

void Shell::input(char c)
{
	size_t size = INPUT_MAX_SIZE + 1;

	switch (c)
	{
		default:
			{
				if (g_inputCursor > INPUT_MAX_SIZE)
				{
					return;
				}
				g_inputBuffer[g_inputCursor] = createEntry(c, g_displayColor);
				g_inputCursor++;
				break;
			}
		case '\b': // Backspace
			{
				if (g_inputCursor == 0)
				{
					g_inputBuffer[g_inputCursor] = TEXT_BLANK;
					return;
				}
				g_inputBuffer[g_inputCursor] = TEXT_BLANK; // Erase current
				g_inputCursor--;
				g_inputBuffer[g_inputCursor] = TEXT_BLANK; // Erase previous
				break;
			}
		case '\n': // Enter
			{
				if (g_inputCursor == 0)
				{
					return;
				}

				String cmd;
				for (size_t i = 0; i < g_inputCursor; i++)
				{
					cmd.append(g_inputBuffer[i]);
				}
				cmd[g_inputCursor] = '\0';
				g_inputBuffer[INPUT_MAX_SIZE] = 0;
				clearInput();
				debugx(&cmd);

				// Print the entire command to the terminal
				debugc(cmd[0]);
				debugc(cmd[1]);
				printf(">>> %s\n", cmd.data());

				CMD::processCmd(cmd);
				return;
			}
	}
	updateCursorPosition();
}
