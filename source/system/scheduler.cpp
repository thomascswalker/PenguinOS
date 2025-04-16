#include <assert.h>
#include <list.h>
#include <memory.h>
#include <pic.h>
#include <scheduler.h>
#include <stdio.h>

using namespace Scheduler;

// Defined as external in 'scheduler.h'
ProcessControlBlock* g_currentProcess = nullptr;

// Process Queue
static ProcessControlBlock* g_queue = nullptr;
static uint32_t				g_taskCount = 0;
static uint32_t				g_currentPID = 0;
static uint32_t				g_irqDisableCounter = 0;

// Defined in scheduler.s
EXTERN void switchProcess(ProcessControlBlock* next);

// Function that will be called prior to the task's func entrypoint
// Unlock the scheduler automatically
static void processStartup()
{
	debug("Starting process...");
	// MUST: All processes at start need to unlock the scheduler
	unlock();
}

ProcessControlBlock* Scheduler::create(EntryPoint func)
{
	ProcessControlBlock* newProcess = (ProcessControlBlock*)malloc(sizeof(ProcessControlBlock));

	if (!newProcess)
	{
		warning("Unable to create new task.");
		return nullptr;
	}

	newProcess->pid = g_currentPID++;
	newProcess->state = Ready;
	newProcess->programCounter = 0;
	newProcess->stackBase = (uintptr_t*)malloc(STACK_SIZE);
	newProcess->stackPointer = (uintptr_t*)((uint32_t)newProcess->stackBase + STACK_SIZE);

	*(--newProcess->stackPointer) = (uintptr_t)func; // eip
	// Static startup function to run before func
	*(--newProcess->stackPointer) = (uintptr_t)processStartup;
	*(--newProcess->stackPointer) = 0; // ebx
	*(--newProcess->stackPointer) = 0; // esi
	*(--newProcess->stackPointer) = 0; // edi
	*(--newProcess->stackPointer) = 0; // ebp

	newProcess->next = nullptr;

	// If the queue has not been initialized, set the new task
	// as the current task.
	if (!g_queue)
	{
		g_queue = newProcess;
		g_currentProcess = g_queue;
	}
	// Otherwise, add it to the end of the queue.
	else
	{
		ProcessControlBlock* current = g_queue;
		while (current->next)
		{
			current = current->next;
		}
		current->next = newProcess;
	}

	g_taskCount++;
	return newProcess;
}

void Scheduler::yield()
{
	lock();
	schedule();
	unlock();
}

void Scheduler::lock()
{
#ifndef SMP
	disableInterrupts();
	g_irqDisableCounter++;
#endif
}

void Scheduler::unlock()
{
#ifndef SMP
	g_irqDisableCounter--;
	if (g_irqDisableCounter == 0)
	{
		enableInterrupts();
	}
#endif
}

void Scheduler::init() { create(nullptr); }

void Scheduler::schedule()
{
	if (!g_currentProcess)
	{
		return;
	}

	ProcessControlBlock* next = g_currentProcess->next;
	if (!next)
	{
		next = g_queue;
	}

	// TODO: There is a problem where the first task in the
	// list is not actually running. For now, we just put a
	// dummy task (nullptr) in the queue to make sure the
	// remaining tasks get executed.
	switchProcess(next);
}
