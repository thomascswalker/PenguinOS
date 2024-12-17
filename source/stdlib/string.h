#pragma once

#include "stdarg.h"
#include "types.h"

size_t strlen(const char* string)
{
	size_t length = 0;
	while (string[length])
	{
		length++;
	}
	return length;
}

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

const char* format(const char* fmt, ...)
{
	const char* out;
	return out;
}