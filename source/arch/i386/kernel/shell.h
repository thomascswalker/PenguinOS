#pragma once

// If on a normal desktop platform, define i386 as to not
// throw an error.
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__) || defined(__apple__)
#define __i386__
#endif

#if !defined(__i386__)
#error "This needs to be compiled with ix86-elf."
#endif

#include <cstring.h>
#include <sys.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_SIZE (VGA_WIDTH * VGA_HEIGHT)

#define DISPLAY_HEIGHT 23

#define INPUT_PROMPT ("/ > ")
#define INPUT_OFFSET ((VGA_WIDTH * (VGA_HEIGHT - 1)) + strlen(INPUT_PROMPT))
#define INPUT_MAX_SIZE 72

// The starting point of the terminal's text buffer in memory.
#define VGA_ADDRESS 0xB8000
#define VGA_BUFFER_START (uint16_t*)VGA_ADDRESS
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

namespace Shell
{

	void init();

	uint16_t createEntry(char character, uint8_t color);
	void	 setForeColor(uint8_t newColor);
	void	 setBackColor(uint8_t newColor);
	void	 clearDisplay();
	void	 clearInput();
	void	 resetColor();

	uint32_t getDisplayPosition();
	void	 setDisplayPosition(uint32_t x, uint32_t y);

	void	 setCursorPosition(int32_t x);
	uint32_t getCursorPosition();
	void	 updateCursorPosition();
	void	 enableCursor(uint32_t start, uint32_t end);
	void	 disableCursor();

	void putNext(char c);
	void putAt(char c, uint32_t pos);

	void insertNewLine();
	void scroll();

	void input(char c);
} // namespace Shell
