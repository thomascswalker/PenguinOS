#include <list.h>
#include <memory.h>
#include <pic.h>
#include <scheduling.h>
#include <stdio.h>
#include <thread.h>

using namespace System;

static List<Process*> g_queue;
static uint32_t		  g_nextPID = 0; // Contains the next free PID

void Scheduler::add(EntryPoint func)
{
	Process* process = (Process*)std::malloc(sizeof(Process));
	process->func = func;								// Set the function pointer
	process->pid = g_nextPID++;							// Assign a unique PID
	process->stack = (uint8_t*)std::malloc(STACK_SIZE); // Allocate a stack for the process

	CPUState* frame = (CPUState*)((uint32_t)process->stack + STACK_SIZE);
	process->state = frame; // Set the CPU state for the process

	// Set the instruction pointer to the function
	frame->eip = (uint32_t)func;

	// Set the stack pointer to the top of the stack
	frame->esp = (uint32_t)(process->stack + STACK_SIZE - sizeof(CPUState));

	// Set the interrupt flag (IF) and reserved flag (RF)
	frame->eFlags = 0x202;
	frame->userEsp = (uint32_t)(process->stack + STACK_SIZE - sizeof(CPUState));

	// Set general-purpose registers
	frame->eax = 0;
	frame->ebx = 0;
	frame->ecx = 0;
	frame->edx = 0;
	frame->edi = 0;
	frame->ebp = 0;
	frame->esi = 0;

	// Set the segment registers
	frame->cs = SEG_KERNEL_CODE;
	frame->ss = SEG_KERNEL_DATA;
	frame->ds = SEG_KERNEL_DATA;
	frame->es = SEG_KERNEL_DATA;
	frame->fs = SEG_KERNEL_DATA;
	frame->gs = SEG_KERNEL_DATA;

	g_queue.addBack(process);
}

void Scheduler::schedule()
{
	if (g_queue.empty())
	{
		return;
	}
	auto curr = g_queue.getFront();
	if (!curr)
	{
		return;
	}
	auto next = curr->getNext();

	if (curr == next)
	{
		return;
	}

	Process* currProc = curr->getValue();
	Process* nextProc = next->getValue();

	debug("Switching processes from %d to %d", currProc->pid, nextProc->pid);
	switchContext(currProc->state, nextProc->state);
	g_queue.rotate(1);
}
