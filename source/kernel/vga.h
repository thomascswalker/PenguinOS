#pragma once

// If on a normal desktop platform, define i386 as to not
// throw an error.
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__) || defined(__apple__)
#define __i386__
#endif

#if !defined(__i386__)
#error "This needs to be compiled with ix86-elf."
#endif

#include <string.h>
#include <sys.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_SIZE (VGA_WIDTH * VGA_HEIGHT)

// The starting point of the terminal's text buffer in memory.
#define VGA_BUFFER_START (uint16_t*)(0xB8000 + 0xC0000000)
#define TEXT_BLANK 1824 // ' ' with black background

/* Hardware text mode color constants. */
enum EVGAColor : uint8_t
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
	VGA_COLOR_YELLOW = 14,
	VGA_COLOR_WHITE = 15,
	VGA_COLOR_DEFAULT = VGA_COLOR_LIGHT_GREY,
	VGA_COLOR_BACKGROUND = VGA_COLOR_BLACK,
};

namespace VGA
{
	static uint16_t* buffer;
	static uint8_t	 color;
	static int32_t	 row;
	static int32_t	 column;

	void	 init();
	uint16_t createEntry(char character, uint8_t color);
	uint32_t getCursorPosition();
	void	 setCursorPosition(int32_t x, int32_t y);
	void	 updateCursorPosition();
	void	 enableCursor(uint32_t start, uint32_t end);
	void	 disableCursor();
	void	 clear();
	void	 setForeColor(uint8_t newColor);
	void	 setBackColor(uint8_t newColor);
	void	 resetColor();
	void	 put(char c);
	void	 remchar();
	void	 insertNewLine();
	void	 scroll();
} // namespace VGA
