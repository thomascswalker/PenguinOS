#pragma once

#include <cstdlib.h>
#include <cstring.h>
#include <idt.h>

#define STACK_SIZE 1024

enum ProcessStatus
{
	Ready,
	Running,
	Dead,
};

typedef void (*ProcessFunc)(void);
struct Process
{
	uint32_t	  pid;
	Registers*	  regs;
	Process*	  next;
	ProcessStatus state;
	~Process() { std::free(regs); }
	bool operator==(const Process& other) const { return pid == other.pid; }
};

EXTERN void kSwitchContext(Registers* regs);

namespace Scheduling
{
	void contextSwitch(Registers* currentRegs, Registers* nextRegs);
	void schedule();
	void createProcess(Process* proc, ProcessFunc func);
	void setCurrentProcess(Process* proc);
	bool getInitialized();

}; // namespace Scheduling