#include <memory.h>
#include <pic.h>
#include <scheduling.h>
#include <stdio.h>

static Process* g_currentProcess = nullptr;
static Process* g_lastProcess = nullptr;
static uint32_t g_procCount = 0;
static size_t	g_nextFreePID = 0;
static bool		g_procInitialized = false;

// https://github.com/szhou42/osdev/blob/master/src/kernel/scheduler/process.c
void Scheduling::contextSwitch(Registers* currentRegs, Registers* nextRegs)
{
	debug("Current Process: %d", g_currentProcess->pid);
	g_currentProcess = g_currentProcess->next;
	// if (g_lastProcess)
	// {
	// 	g_lastProcess->regs->eax = currentRegs->eax;
	// 	g_lastProcess->regs->ebx = currentRegs->ebx;
	// 	g_lastProcess->regs->ecx = currentRegs->ecx;
	// 	g_lastProcess->regs->edx = currentRegs->edx;
	// 	g_lastProcess->regs->esi = currentRegs->esi;
	// 	g_lastProcess->regs->edi = currentRegs->edi;
	// 	g_lastProcess->regs->ebp = currentRegs->ebp;
	// 	g_lastProcess->regs->esp = currentRegs->userEsp;
	// 	g_lastProcess->regs->eip = currentRegs->eip;
	// }
	// g_currentProcess = g_currentProcess->next;
	// PIC::sendEOI(0);

	// kSwitchContext(nextRegs);
}

void Scheduling::schedule()
{
	debug("Scheduling");
	Process* prev = g_currentProcess;
	Process* next = g_currentProcess->next;
	while (next->state != Ready)
	{
		next = next->next;
	}
	g_currentProcess = g_currentProcess->next;
	contextSwitch(prev->regs, g_currentProcess->regs);
}

void Scheduling::createProcess(Process* proc, ProcessFunc func)
{
	// Place the Registers structure at the top of the stack.
	// The stack grows downward, so subtract regCount from the top.
	uint8_t*   stack = (uint8_t*)std::malloc(STACK_SIZE);
	uint8_t*   stackTop = stack + STACK_SIZE - sizeof(Registers);
	Registers* regs = reinterpret_cast<Registers*>(stackTop);

	// Initialize "common stub" registers.
	regs->ds = 0x10; // Data segment selector (example value)
	regs->edi = 0;
	regs->esi = 0;
	regs->ebp = 0;
	regs->esp = 0; // Placeholder; will be updated on context switch.
	regs->ebx = 0;
	regs->edx = 0;
	regs->ecx = 0;
	regs->eax = 0;

	// Initialize "wrapper function" registers.
	regs->intNo = 0;
	regs->errCode = 0;

	// Initialize "interrupt" pushed registers.
	regs->eip = reinterpret_cast<uint32_t>(func); // Entry point for the process.
	regs->cs = 0x08;	  // Code segment selector (typically 0x08 for kernel code).
	regs->eFlags = 0x202; // EFLAGS: 0x200 sets the interrupt flag.
	regs->userEsp = 0;	  // Not used for kernel-mode processes, usually.
	regs->ss = 0x10;	  // Stack segment selector.

	// Save the pointer to the Registers structure in the PCB.
	proc->regs = regs;
}

void Scheduling::setCurrentProcess(Process* proc)
{
	g_currentProcess = proc;
	g_procInitialized = true;
}

bool Scheduling::getInitialized() { return g_procInitialized; }
