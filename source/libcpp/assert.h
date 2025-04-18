#pragma once

#include <cstdio.h>

#define ASSERT(x)                                                                             \
	if (!(x))                                                                                 \
	{                                                                                         \
		panic("Assertion failed: %s.\n\t\t  File:%s\n\t\t  Line:%d", #x, __FILE__, __LINE__); \
	}
