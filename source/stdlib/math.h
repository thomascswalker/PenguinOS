#pragma once

#include <stdint.h>

#define PI 3.141592
#define PI2 (PI * 2.0)
#define HALF_PI (PI / 2.0)

uint32_t ceil(uint32_t a, uint32_t b)
{
	return (((a + b) - 1) / b);
}