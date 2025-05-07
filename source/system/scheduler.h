#pragma once

#include <idt.h>

#define STACK_SIZE 0x400

using EntryPoint = void (*)();

namespace Scheduler
{
	enum TaskState
	{
		Ready,
		Running,
		Waiting,
		Terminated
	};

	struct ProcessControlBlock
	{
		uint32_t			 pid;			 // Process ID
		TaskState			 state;			 // Process state
		uintptr_t			 programCounter; // Program Counter
		uintptr_t*			 stackBase;		 // Top of the stack
		uintptr_t*			 stackPointer;	 // Stack Pointer
		uint32_t			 flags;			 // Status/Flags register
		ProcessControlBlock* next;			 // Pointer to the next task in the list
	};
	using PCB = ProcessControlBlock;

	ProcessControlBlock* create(EntryPoint func);
	void				 init();
	void				 schedule();
	void				 yield();
	void				 lock();
	void				 unlock();
}; // namespace Scheduler

EXTERN Scheduler::ProcessControlBlock* g_currentProcess;