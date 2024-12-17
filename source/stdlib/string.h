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

const char* format(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	// Create the output character buffer
	const char* out;

	// Direct copy from source to dest
	mem_copy((void*)out, (void*)fmt, str_length(fmt));

	va_end(args);

	return fmt;
}