#pragma once

#include <memory.h>
#include <stdarg.h>
#include <types.h>

// Returns the length of `string`.
size_t strlen(const char* string)
{
	size_t length = 0;
	while (string[length])
	{
		length++;
	}
	return length;
}

// Returns true if both strings are equal. False otherwise.
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

// Reverses, in-place, the specified `string`.
void strrev(char* string)
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

// Returns the number of decimals in the specified value.
// e.g. `12345` would return `5`.
int8_t decimal_count(int32_t value)
{
	// If the value is between 0 and 9, there's only 1 digit.
	if (value >= 0 && value <= 9)
	{
		return 1;
	}

	// Increment i while dividing the value by 10 until it's
	// equal to 0.
	int8_t i = 0;
	do
	{
		i++;
	}
	while (value /= 10 > 0);

	return i;
}

// Converts the specified integer to a string.
void itos(int32_t value, char* buffer)
{
	// If the value is 0, just simply return 0 as a string.
	if (value == 0)
	{
		buffer[0] = '0';
		buffer[1] = '\0';
		return;
	}

	char*  ptr = buffer; // Pointer to the current char
	size_t size = 0;	 // Accumulate final string size

	// Loop through the number until it's equal to 0. Each
	// iteration divide by 10 and the remainder will be the
	// current digit.
	do
	{
		// Get the remainder of the current value / 10.
		// Add to the char '0' to get the correct ASCII
		// char value.
		*ptr = (char)('0' + (value % 10));

		// Actually divide `value` by 10.
		value /= 10;

		// Move to the next character and increment size.
		ptr++;
		size++;
	}
	while (value != 0);

	// Reverse the string (since it was built in reverse
	// order).
	strrev(buffer);

	// Terminate
	buffer[size] = '\0';
}