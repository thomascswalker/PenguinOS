#include <assert.h>
#include <list.h>
#include <memory.h>
#include <pic.h>
#include <scheduler.h>
#include <stdio.h>

using namespace System;

Task*			g_queue;
Task*			g_currentTask = nullptr;
uint32_t		g_taskCount = 0;
static uint32_t g_currentPID = 0;

// Defined in scheduler.s
EXTERN void switchTask(Task* next);

void idle()
{
	while (1)
	{
		// Do nothing, just yield to other tasks.
		printf("Idling...\n");
		System::schedule();
	}
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
	*(--newTask->stackPointer) = 0;				  // ebx
	*(--newTask->stackPointer) = 0;				  // esi
	*(--newTask->stackPointer) = 0;				  // edi
	*(--newTask->stackPointer) = 0;				  // ebp

	if (!g_queue)
	{
		g_queue = newTask;
		g_currentTask = g_queue;
	}
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

void System::init() { create(&idle); }

void System::schedule()
{
	if (g_taskCount == 0)
	{
		return;
	}

	Task* next = g_currentTask->next;
	if (!next)
	{
		next = g_queue; // Loop back to the start of the queue
	}

	switchTask(next);
}
