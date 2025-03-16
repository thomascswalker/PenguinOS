#include <cstdlib.h>
#include <stdint.h>
#include <thread.h>

using namespace System;

Thread::Thread(Function<void>* func)
{
	// Initialize a new stack
	m_stack = (uint8_t*)std::malloc(STACK_SIZE);
	memset(m_stack, 0, STACK_SIZE);

	// Position m_state at the proper address at the top of the allocated stack, subtracting the
	// size of CPUState.
	m_state = (CPUState*)((uint32_t)m_stack + STACK_SIZE - sizeof(CPUState));

	m_state->ds = SEG_KERNEL_DATA;
	m_state->es = SEG_KERNEL_DATA;
	m_state->fs = SEG_KERNEL_DATA;
	m_state->gs = SEG_KERNEL_DATA;

	m_state->edi = 0;
	m_state->esi = 0;
	m_state->ebp = 0;
	m_state->esp = (uint32_t)m_stack; // Set stack pointer
	m_state->ebx = 0;
	m_state->edx = 0;
	m_state->ecx = 0;
	m_state->eax = 0;

	m_state->intNo = 0;
	m_state->errCode = 0;

	m_state->eip = (uint32_t)func; // Set instruction pointer
	m_state->cs = SEG_KERNEL_CODE; // Set segment registers
	m_state->eFlags = 0x202;	   // Set flags
	m_state->userEsp = 0;		   // Set user stack pointer
	m_state->ss = SEG_KERNEL_DATA; // Set stack segment
}