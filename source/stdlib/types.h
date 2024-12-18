#pragma once

#include "stdbool.h"

typedef signed char		   int8;
typedef signed short	   int16;
typedef signed int		   int32;
typedef unsigned char	   uint8;
typedef unsigned short	   uint16;
typedef unsigned int	   uint32;
typedef unsigned long long size_t;

bool is_ascii(char c)
{
	return ((c >= 0) && (c <= 127));
}

bool is_number(char c)
{
	return ((c >= 48) && (c <= 57));
}