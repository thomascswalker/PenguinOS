#include <list.h>
#include <memory.h>
#include <pic.h>
#include <scheduling.h>
#include <stdio.h>
#include <thread.h>

using namespace System;

static Process* g_processQueue = nullptr;	// List of processes in the system.
static Process* g_currentProcess = nullptr; // The currently running process.
static uint32_t g_nextPID = 0;				// Contains the next free PID

static void idle()
{
	while (1)
	{
		// Do nothing, just idle
	}
}
static bool g_initialized = false;

void System::Scheduler::init()
{
	add(idle);
	g_initialized = true;
}

void Scheduler::add(EntryPoint func)
{
	Process* process = (Process*)std::malloc(sizeof(Process));
	process->func = func;								// Set the function pointer
	process->next = nullptr;							// Initialize next as nullptr
	process->pid = g_nextPID++;							// Assign a unique PID
	process->stack = (uint8_t*)std::malloc(STACK_SIZE); // Allocate a stack for the process

	CPUState* frame = &process->state;

	// Set the instruction pointer to the function
	frame->eip = (uint32_t)func;

	// Set the stack pointer to the top of the stack
	frame->esp = (uint32_t)(process->stack + STACK_SIZE);

	// Set the interrupt flag (IF) and reserved flag (RF)
	frame->eFlags = 0x202;
	frame->intNo = 0;
	frame->errCode = 0;
	frame->userEsp = (uint32_t)(process->stack + STACK_SIZE);

	// Set general-purpose registers
	frame->edi = 0;
	frame->esi = 0;
	frame->ebp = 0;
	frame->ebx = 0;
	frame->edx = 0;
	frame->ecx = 0;
	frame->eax = 0;

	// Set the segment registers
	frame->cs = SEG_KERNEL_CODE;
	frame->ss = SEG_KERNEL_DATA;
	frame->ds = SEG_KERNEL_DATA;
	frame->es = SEG_KERNEL_DATA;
	frame->fs = SEG_KERNEL_DATA;
	frame->gs = SEG_KERNEL_DATA;

	// If the queue is empty, set the first process in the queue and set the current process
	// to the one we just created.
	if (g_processQueue == nullptr)
	{
		g_processQueue = process;
		g_currentProcess = process;
	}
	// Otherwise, traverse to the end of the queue and add the new process there.
	else
	{
		Process* current = g_processQueue;
		while (current->next != nullptr)
		{
			current = current->next;
		}
		current->next = process;
	}
}

void Scheduler::schedule()
{
	if (!g_initialized)
	{
		return;
	}
	if (!g_currentProcess)
	{
		return;
	}

	Process* nextProcess = g_currentProcess->next;
	if (nextProcess == nullptr)
	{
		nextProcess = g_processQueue; // Loop back to the start of the queue
	}
	debug("Switching processes from %d to %d", g_currentProcess->pid, nextProcess->pid);
	switchContext(&g_currentProcess->state, &nextProcess->state);
	g_currentProcess = nextProcess;
}
