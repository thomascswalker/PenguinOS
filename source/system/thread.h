#pragma once

#include <functional.h>
#include <sys.h>

#define SEG_KERNEL_CODE 0x08
#define SEG_KERNEL_DATA 0x10
#define SEG_USER_CODE 0x1B
#define SEG_USER_DATA 0x23

#define STACK_SIZE 1024

typedef Function<void> EntryPoint;

namespace System
{
	class Thread
	{
		uint8_t*  m_stack;
		CPUState* m_state;

	public:
		Thread() = default;
		Thread(Function<void>* func);

		uint8_t*  stack() const { return m_stack; }
		CPUState* state() const { return m_state; }
	};
} // namespace System