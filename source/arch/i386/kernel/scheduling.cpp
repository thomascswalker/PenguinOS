#include <memory.h>
#include <pic.h>
#include <scheduling.h>
#include <stdio.h>

static Task* g_taskList = nullptr;
static Task* g_currentTask = nullptr;

void Scheduling::schedule()
{
	Task* next;
	if (!g_currentTask)
	{
		next = g_taskList;
	}
	else
	{
		next = g_currentTask->next ? g_currentTask->next : g_taskList;
	}

	if (!next)
	{
		// asm("sti\nhlt");
		return;
	}

	Task* prev = g_currentTask;
	g_currentTask = next;
	switchTo(prev, next);
}

void Scheduling::createTask(TaskFunc func)
{
	Task* newTask = new Task();
	if (!newTask)
	{
		error("Unable to create new task.");
		return;
	}

	uint8_t* stack = (uint8_t*)std::malloc(STACK_SIZE);
	if (!stack)
	{
		error("Unable to allocate new stack.");
		return;
	}

	newTask->stack = stack;
	uint32_t* sp = (uint32_t*)(stack + STACK_SIZE);
	// *(--sp) = (uint32_t)&exitTask;
	*(--sp) = (uint32_t)func;
	*(--sp) = 0; // EDI
	*(--sp) = 0; // ESI
	*(--sp) = 0; // EBX
	*(--sp) = 0; // EBP

	newTask->sp = sp;

	// Push the new task onto the front of the stack list
	newTask->next = g_taskList;
	g_taskList = newTask;
}

void Scheduling::removeTask(Task* task)

{
	if (g_taskList == task)
	{
		g_taskList = task->next;
	}
	else
	{
		Task* prev = g_taskList;
		while (prev && prev->next != task)
		{
			prev = prev->next;
		}
		if (prev)
		{
			prev->next = task->next;
		}
	}
	std::free(task->stack);
	std::free(task);
}

void Scheduling::exitTask()
{
	if (g_currentTask)
	{
		Task* finished = g_currentTask;
		removeTask(finished);
		g_currentTask = nullptr;
	}
	schedule();
	while (1)
	{
	}
}
