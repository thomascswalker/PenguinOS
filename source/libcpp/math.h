#pragma once

#include <stdint.h>

#define PI 3.141592
#define PI2 (PI * 2.0)
#define HALF_PI (PI / 2.0)

#define CEILDIV(a, b) (((a + b) - 1) / b)

namespace std
{
	template <typename T>
	T min(T a, T b)
	{
		return (a < b) ? a : b;
	}

	template <typename T>
	T max(T a, T b)
	{
		return (a > b) ? a : b;
	}
} // namespace std