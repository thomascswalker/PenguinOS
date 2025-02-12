#pragma once

#include <ctype.h>
#include <stdarg.h>

// 	Copies one buffer to another.
static void* memcpy(void* dest, void* source, size_t size)
{
	uint8_t*	   p_dest = (uint8_t*)dest;
	const uint8_t* p_source = (uint8_t*)source;

	while (size--)
	{
		*p_dest++ = *p_source++;
	}
	return dest;
}

// Fills a buffer with a repeated byte.
static void* memset(void* dest, uint8_t value, int32_t size)
{
	char* tmp = (char*)dest;
	for (int32_t i = 0; i < size; i++)
	{
		tmp[i] = value;
	}
	return tmp;
}

// Fills a buffer with a repeated byte.
static uint16_t* wmemset(uint16_t* dest, uint16_t value, int32_t size)
{
	for (int32_t i = 0; i < size; i++)
	{
		dest[i] = value;
	}
	return dest;
}

// Returns the length of the string.
static size_t strlen(const char* string)
{
	size_t length = 0;
	while (string[length])
	{
		length++;
	}
	return length;
}

// Compares two strings.
static bool strcmp(const char* lhs, const char* rhs)
{
	if (strlen(lhs) != strlen(rhs))
	{
		return false;
	}

	for (size_t i = 0; i < strlen(lhs); i++)
	{
		if (lhs[i] != rhs[i])
		{
			return false;
		}
	}

	return true;
}

// Copies one string to another
static char* strcpy(char* dest, const char* source)
{
	char* temp = dest;
	while (*dest++ = *source++)
	{
	}
	return temp;
}

// Appends one string to another
static char* strcat(char* dest, const char* source)
{
	size_t i = 0, j = 0;
	while (dest[i] != '\0')
	{
		i++;
	}
	while (dest[j] != '\0')
	{
		dest[i + j] = source[j];
	}
	dest[i + j] = '\0';

	return dest;
}

static char toupper(char c) { return c + ('A' - 'a'); }

static char tolower(char c) { return c - ('A' + 'a'); }

// Reverses, in-place, the specified `string`.
static void strrev(char* string)
{
	int start = 0;
	int end = strlen(string) - 1;
	while (start < end)
	{
		char c = string[start];
		string[start] = string[end];
		string[end] = c;

		start++;
		end--;
	}
}

static void strapp(char* source, const char* string, char* dest)
{
	strcpy(dest, source);
	dest += sizeof(source);
	strcpy(dest, string);
}

static void strprep(char* source, const char* string, char* dest)
{
	strcpy(dest, string);
	dest += sizeof(string);
	strcpy(dest, source);
}

static void wtoc(char* dest, wchar_t* source, size_t length)
{
	uint32_t i = 0;
	while (length--)
	{
		*dest++ = source[i]; // | (source[1] << 8);
		i += 2;
	}
}