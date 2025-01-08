#pragma once

#include <stdint.h>

#define PI 3.141592
#define PI2 (PI * 2.0)
#define HALF_PI (PI / 2.0)

static inline int32_t ceil(int32_t a, int32_t b)
{
	return (((a + b) - 1) / b);
}