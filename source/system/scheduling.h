#pragma once

#include <array.h>
#include <cstdlib.h>
#include <cstring.h>
#include <idt.h>
#include <process.h>

namespace System
{
	/**
	 * @brief Switches the CPU context from the previous state to the next state.
	 *
	 * Defined in 'scheduling.s'
	 *
	 * @param prev A pointer to the CPUState structure representing the current
	 *             state of the CPU that will be saved.
	 * @param next A pointer to the CPUState structure representing the next state
	 *             of the CPU that will be restored.
	 */
	EXTERN void switchContext(CPUState* prev, CPUState* next);

	/**
	 * @brief The Scheduler class manages the scheduling of processes in the system.
	 */
	namespace Scheduler
	{
		void init();
		/**
		 * @brief Adds a new process to the scheduler.
		 * @param func The entry point function for the new process.
		 */
		void add(EntryPoint func);

		/**
		 * @brief Switches to the next process in the queue.
		 */
		void schedule();
	}; // namespace Scheduler
}; // namespace System