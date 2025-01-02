#pragma once

#include <ctype.h>
#include <stdarg.h>

// 	Copies one buffer to another.
void* memcpy(void* dest, void* source, size_t size)
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
void* memset(void* dest, uint8_t value, int32_t size)
{
	char* tmp = (char*)dest;
	for (int32_t i = 0; i < size; i++)
	{
		tmp[i] = value;
	}
	return tmp;
}

// Fills a buffer with a repeated byte.
uint16_t* wmemset(uint16_t* dest, uint16_t value, int32_t size)
{
	for (int32_t i = 0; i < size; i++)
	{
		dest[i] = value;
	}
	return dest;
}

// Returns the length of the string.
size_t strlen(const char* string)
{
	size_t length = 0;
	while (string[length])
	{
		length++;
	}
	return length;
}

// Compares two strings.
bool strcmp(const char* lhs, const char* rhs)
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
char* strcpy(char* dest, const char* source)
{
	char* temp = dest;
	while (*dest++ = *source++)
	{
	}
	return temp;
}

// Appends one string to another
char* strcat(char* dest, const char* source)
{
	size_t i, j;
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

// Reverses, in-place, the specified `string`.
void reverse(char* string)
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
