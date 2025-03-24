#include <assert.h>
#include <list.h>
#include <memory.h>
#include <pic.h>
#include <scheduler.h>
#include <stdio.h>

using namespace System;

// Defined as external in 'scheduler.h'
Task* g_currentTask = nullptr;

// Task Queue
static Task*	g_queue = nullptr;
static uint32_t g_taskCount = 0;
static uint32_t g_currentPID = 0;

static uint32_t IRQ_disable_counter = 0;

// Defined in scheduler.s
EXTERN void switchTask(Task* next);


// Function that will be called prior to the task's func entrypoint
// Unlock the scheduler automatically
static void processStartup(void)
{
	// MUST: All processes at start need to unlock the scheduler
	unlock_scheduler();
}

Task* System::create(EntryPoint func)
{
	Task* newTask = (Task*)std::malloc(sizeof(Task));

	if (!newTask)
	{
		warning("Unable to create new task.");
		return nullptr;
	}

	newTask->pid = g_currentPID++;
	newTask->state = Ready;
	newTask->programCounter = 0;
	newTask->stackTop = (uintptr_t*)std::malloc(STACK_SIZE);
	newTask->stackPointer = (uintptr_t*)((uint32_t)newTask->stackTop + STACK_SIZE);

	*(--newTask->stackPointer) = (uintptr_t)func; // eip
	*(--newTask->stackPointer) = (uintptr_t)processStartup; // Static startup function to run before func
	*(--newTask->stackPointer) = 0;				  // ebx
	*(--newTask->stackPointer) = 0;				  // esi
	*(--newTask->stackPointer) = 0;				  // edi
	*(--newTask->stackPointer) = 0;				  // ebp

	newTask->next = nullptr;

	// If the queue has not been initialized, set the new task
	// as the current task.
	if (!g_queue)
	{
		g_queue = newTask;
		g_currentTask = g_queue;
	}
	// Otherwise, add it to the end of the queue.
	else
	{
		Task* current = g_queue;
		while (current->next)
		{
			current = current->next;
		}
		current->next = newTask;
	}

	g_taskCount++;
	return newTask;
}

void System::yield(void)
{
	lock_scheduler();
	schedule();
	unlock_scheduler();
}

void System::lock_scheduler(void)
{
#ifndef SMP
	disableInterrupts();
	IRQ_disable_counter++;
#endif
}

void System::unlock_scheduler(void)
{
#ifndef SMP
	IRQ_disable_counter--;
	if(IRQ_disable_counter == 0) {
		enableInterrupts();
	}
#endif
}


void System::init() { create(nullptr); }

void System::schedule()
{
	if (!g_currentTask)
	{
		return;
	}

	Task* next = g_currentTask->next;
	if (!next)
	{
		next = g_queue;
	}

	// TODO: There is a problem where the first task in the
	// list is not actually running. For now, we just put a
	// dummy task (nullptr) in the queue to make sure the
	// remaining tasks get executed.
	switchTask(next);
}
