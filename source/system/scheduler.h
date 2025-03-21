#pragma once

#include <array.h>
#include <cstdlib.h>
#include <cstring.h>
#include <gdt.h>
#include <idt.h>

#define STACK_SIZE 0x400

using EntryPoint = void (*)();

namespace System
{
	enum TaskState
	{
		Ready,
		Running,
		Waiting,
		Terminated
	};

	struct Task
	{
		uint32_t   pid;			   // Process ID
		TaskState  state;		   // Process state
		uintptr_t  programCounter; // Program Counter
		uintptr_t* stackPointer;   // Stack Pointer
		uintptr_t* stackTop;	   // Stack Pointer Base for free();
		uint32_t   flags;		   // Status/Flags register
		Task*	   next;		   // Pointer to the next task in the list
	};

	Task* create(EntryPoint func);
	void  init();
	void  schedule();
}; // namespace System

EXTERN System::Task* g_currentTask;
EXTERN uint32_t		 g_taskCount;