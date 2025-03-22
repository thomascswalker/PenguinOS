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

	struct Process
	{
		uint32_t   pid;			   // Process ID
		TaskState  state;		   // Process state
		uintptr_t  programCounter; // Program Counter
		uintptr_t* stackBase;	   // Top of the stack
		uintptr_t* stackPointer;   // Stack Pointer
		uint32_t   flags;		   // Status/Flags register
		Process*   next;		   // Pointer to the next task in the list
	};

	Process* create(EntryPoint func);
	void	 init();
	void	 schedule();
}; // namespace System

EXTERN System::Process* g_currentProcess;