#pragma once

#include "stdarg.h"
#include "stdmem.h"
#include "types.h"

size_t str_length(const char* string)
{
	size_t length = 0;
	while (string[length])
	{
		length++;
	}
	return length;
}

bool str_equal(const char* lhs, const char* rhs)
{
	if (str_length(lhs) != str_length(rhs))
	{
		return false;
	}

	for (size_t i = 0; i < str_length(lhs); i++)
	{
		if (lhs[i] != rhs[i])
		{
			return false;
		}
	}

	return true;
}

void str_reverse(char* str)
{
	int start = 0;
	int end = str_length(str) - 1;
	while (start < end)
	{
		char c = str[start];
		str[start] = str[end];
		str[end] = c;

		start++;
		end--;
	}
}

// Returns the number of decimals in the specified value.
// e.g. `12345` would return `5`.
int8 decimal_count(int32 value)
{
	// If the value is between 0 and 9, there's only 1 digit.
	if (value >= 0 && value <= 9)
	{
		return 1;
	}

	// Increment i while dividing the value by 10 until it's
	// equal to 0.
	int8 i = 0;
	do
	{
		i++;
	}
	while (value /= 10 > 0);

	return i;
}

// Converts the specified integer to a string.
char* itos(int32 value)
{
	// If the value is 0, just simply return 0 as a string.
	if (value == 0)
	{
		return "0";
	}

	static char buffer[21];	  // The output string
	char*		ptr = buffer; // Pointer to the current char
	size_t		size = 0;	  // Accumulate final string size

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
	str_reverse(buffer);

	// Terminate
	buffer[size] = '\0';
	return buffer;
}