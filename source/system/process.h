#pragma once

#include <functional.h>
#include <stdint.h>
#include <sys.h>

namespace System
{
	typedef void (*EntryPoint)();

	struct Process
	{
		uint32_t   pid;
		EntryPoint func;
		CPUState*  frame;
		Process*   next;
		uint8_t*   stack;
	};
} // namespace System
