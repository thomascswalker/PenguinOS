#include <assert.h>
#include <list.h>
#include <memory.h>
#include <pic.h>
#include <scheduler.h>
#include <stdio.h>

using namespace System;

// Defined as external in 'scheduler.h'
Process* g_currentProcess = nullptr;

// Task Queue
static Process* g_processQueue = nullptr;
static uint32_t g_processCount = 0;
static uint32_t g_currentPID = 0;

// Defined in scheduler.s
EXTERN void switchProcess(Process* next);

Process* System::create(EntryPoint func)
{
	Process* newProcess = (Process*)std::malloc(sizeof(Process));

	if (!newProcess)
	{
		warning("Unable to create new task.");
		return nullptr;
	}

	newProcess->pid = g_currentPID++;
	newProcess->state = Ready;
	newProcess->programCounter = 0;
	newProcess->stackBase = (uintptr_t*)std::malloc(STACK_SIZE);
	newProcess->stackPointer = (uintptr_t*)((uint32_t)newProcess->stackBase + STACK_SIZE);

	*(--newProcess->stackPointer) = (uintptr_t)func; // eip
	*(--newProcess->stackPointer) = 0;				 // ebx
	*(--newProcess->stackPointer) = 0;				 // esi
	*(--newProcess->stackPointer) = 0;				 // edi
	*(--newProcess->stackPointer) = 0;				 // ebp

	newProcess->next = nullptr;

	// If the queue has not been initialized, set the new task
	// as the current task.
	if (!g_processQueue)
	{
		g_processQueue = newProcess;
		g_currentProcess = g_processQueue;
	}
	// Otherwise, add it to the end of the queue.
	else
	{
		Process* current = g_processQueue;
		while (current->next)
		{
			current = current->next;
		}
		current->next = newProcess;
	}

	g_processCount++;
	return newProcess;
}

void System::init() { create(nullptr); }

void System::schedule()
{
	if (!g_currentProcess)
	{
		return;
	}

	Process* next = g_currentProcess->next;
	if (!next)
	{
		next = g_processQueue;
	}

	// TODO: There is a problem where the first task in the
	// list is not actually running. For now, we just put a
	// dummy task (nullptr) in the queue to make sure the
	// remaining tasks get executed.
	switchProcess(next);
}
