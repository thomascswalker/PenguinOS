#pragma once

#include <stdint.h>

#define PI 3.141592
#define PI2 (PI * 2.0)
#define HALF_PI (PI / 2.0)

#define CEILDIV(a, b) (((a + b) - 1) / b)

namespace std
{
	template <typename ValueType>
	ValueType min(ValueType a, ValueType b)
	{
		return (a < b) ? a : b;
	}

	template <typename ValueType>
	ValueType max(ValueType a, ValueType b)
	{
		return (a > b) ? a : b;
	}
} // namespace std