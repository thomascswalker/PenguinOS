#pragma once

#include "stdarg.h"
#include "stdmem.h"
#include "types.h"

typedef int (*buffer_write_func)(char*);

struct stringstream
{
	size_t			  buffer_length; // Size of the stream
	size_t			  buffer_pos;	 // Current position of the cursor
	char*			  buffer;		 // The actual character buffer
	buffer_write_func func;			 // The write function which writes to this buffer
};
typedef struct stringstream stringstream_t;

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
