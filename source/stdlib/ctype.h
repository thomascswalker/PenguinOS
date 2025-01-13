#pragma once

#include <stdbool.h>
#include <stdint.h>

#define EXTERN extern "C"

typedef unsigned long long size_t;

static bool isascii(int32_t c)
{
	return ((c >= 0) && (c <= 255));
}

static bool isintegral(int32_t c)
{
	return ((c >= 48) && (c <= 57));
}
