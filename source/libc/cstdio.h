#pragma once

#include <cstdlib.h>
#include <stdarg.h>

// Printing

enum FormatSpec
{
	FMT_CHAR = 'c',
	FMT_STRING = 's',
	FMT_INT = 'd',
	FMT_HEX = 'x'
};

void putc(char c);
char getc();

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

#define debugd(v) debug("%s: %d", #v, v)
#define debugx(v) debug("%s: %x", #v, v)
#define debugs(v) debug("%s: %s", #v, v)
#define debugc(v) debug("%s: %c", #v, v)

// Files

struct File
{
	char*	name;	// File name
	char*	buffer; // Buffer for file I/O
	size_t	size;	// Size of the buffer
	int32_t fd;		// File descriptor
	size_t	pos;	// Current position in the file
};

struct FileStat
{
	size_t size; // Size of the file
};

File*  fopen(const char* filename);
size_t fread(File* stream, void* buffer, size_t size);
void   fclose(File* stream);
void   fstat(int32_t fd, FileStat* buffer);