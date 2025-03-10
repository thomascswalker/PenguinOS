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

typedef void (*TaskFunc)(void);
struct Task
{
	void*	  stack;
	uint32_t* sp;
	Task*	  next;
};

namespace Scheduling
{
	// Defined in 'scheduling.s'
	extern "C" void switchTo(Task* prev, Task* next);

	void schedule();
	void createTask(TaskFunc func);
	void removeTask(Task* task);
	void exitTask();
}; // namespace Scheduling