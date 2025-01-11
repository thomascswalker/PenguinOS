#pragma once

#include <stdarg.h>
#include <stdlib.h>
#include <vga.h>

// Printing

enum format_spec
{
	FMT_CHAR = 'c',
	FMT_STRING = 's',
	FMT_INT = 'd',
	FMT_HEX = 'x',
};
typedef enum format_spec format_spec_t;

void	 print(const char* str);
void	 println(const char* str);
uint32_t sprintf(char* stream, const char* format, va_list args);
void	 printf(const char* format, ...);

void debug(const char* format, ...);
void info(const char* format, ...);
void warning(const char* format, ...);
void error(const char* format, ...);
void success(const char* format, ...);
void panic(const char* format, ...);