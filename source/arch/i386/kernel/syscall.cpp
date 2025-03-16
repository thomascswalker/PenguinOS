#include <memory.h>
#include <pic.h>
#include <pit.h>
#include <stdio.h>
#include <syscall.h>

static SysCallFunc syscalls[] = {
	sysFork,
	sysExit,
	sysWait,
	sysPipe,
	sysRead,
	sysKill,
	sysExec,
	sysFstat,
	sysChdir,
	sysDup,
	sysGetpid,
	sysSbrk,
	sysSleep,
	sysUptime,
	sysOpen,
	sysWrite,
	sysMknod,
	sysUnlink,
	sysLink,
	sysMkdir,
	sysClose,
};

/*
Dispatches the arguments held in registers `ebx`, `ecx`,
 `edx`, `esi`, `edi` to the respective system call. The
 system call code is held in `eax` and is used as an index to
 the `syscalls` array containing a function pointer to each
 system call.
*/
void sysCallDispatcher(CPUState* regs)
{
	uint32_t count = sizeof(syscalls) / sizeof(SysCallFunc);

	// System call code is stored in EAX
	uint32_t sysNo = regs->eax;
	if (sysNo >= count)
	{
		panic("Invalid syscall %d.", sysNo);
		return;
	}

	switch (sysNo)
	{
		case SYSCALL_SLEEP:
			sysSleep(regs);
			break;
		default:
			warning("Not implemented yet.");
			break;
	}
}

int32_t sysFork(CPUState* regs) { return 0; }
int32_t sysExit(CPUState* regs) { return 0; }
int32_t sysWait(CPUState* regs) { return 0; }
int32_t sysPipe(CPUState* regs) { return 0; }
int32_t sysRead(CPUState* regs) { return 0; }
int32_t sysKill(CPUState* regs) { return 0; }
int32_t sysExec(CPUState* regs) { return 0; }
int32_t sysFstat(CPUState* regs) { return 0; }
int32_t sysChdir(CPUState* regs) { return 0; }
int32_t sysDup(CPUState* regs) { return 0; }
int32_t sysGetpid(CPUState* regs) { return 0; }
int32_t sysSbrk(CPUState* regs) { return 0; }
int32_t sysSleep(CPUState* regs)
{
	// Get the milliseconds to sleep for from the ecx
	// register.
	uint32_t ms = regs->ebx;

	// Set the current SLEEP_TICK value to the milliseconds
	// we're sleeping for.
	SLEEP_TICK = ms;
	// Track seconds that have passed.
	uint32_t seconds = 0;

	// Sleep while SLEEP_TICK is greater than 0.
	while (SLEEP_TICK)
	{
		// Increment seconds.
		if (SLEEP_TICK % PIT_FREQ == 0)
		{
			seconds++;
		}
		// Enable interrupts, halt, disable interrupts.
		asm("sti\nhlt\ncli");
	}

	return 0;
}
int32_t sysUptime(CPUState* regs) { return 0; }
int32_t sysOpen(CPUState* regs) { return 0; }
int32_t sysWrite(CPUState* regs) { return 0; }
int32_t sysMknod(CPUState* regs) { return 0; }
int32_t sysUnlink(CPUState* regs) { return 0; }
int32_t sysLink(CPUState* regs) { return 0; }
int32_t sysMkdir(CPUState* regs) { return 0; }
int32_t sysClose(CPUState* regs) { return 0; }
