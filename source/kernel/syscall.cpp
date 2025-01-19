#include <pic.h>
#include <pit.h>
#include <pmm.h>
#include <stdio.h>
#include <syscall.h>

/*
Dispatches the arguments held in registers `ebx`, `ecx`,
 `edx`, `esi`, `edi` to the respective system call. The
 system call code is held in `eax` and is used as an index to
 the `syscalls` array containing a function pointer to each
 system call.
*/
void sysCallDispatcher(Registers regs)
{
	uint32_t count = sizeof(syscalls) / sizeof(SysCallFunc);

	// System call code is stored in EAX
	if (regs.eax >= count)
	{
		panic("Invalid syscall %d.", regs.eax);
		return;
	}

	// Arguments for the respective system call function are
	// in order from ebx, ecx, edx, esi, edi.
	SysCallFunc syscall = syscalls[regs.eax];
	uint32_t	ret;
	asm("push %1\n"
		"push %2\n"
		"push %3\n"
		"push %4\n"
		"push %5\n"

		"call %6\n"

		"pop %%eax\n"
		"pop %%ebx\n"
		"pop %%ecx\n"
		"pop %%edx\n"
		"pop %%esi\n"
		"pop %%edi\n"

		"iret\n"
		: "=a"(ret)
		: "r"(regs.edi), "r"(regs.esi), "r"(regs.edx), "r"(regs.ecx), "r"(regs.ebx), "r"(syscall));
}

int32_t sysFork(SysCallRegisters regs)
{
	return 0;
}
int32_t sysExit(SysCallRegisters regs)
{
	return 0;
}
int32_t sysWait(SysCallRegisters regs)
{
	return 0;
}
int32_t sysPipe(SysCallRegisters regs)
{
	return 0;
}
int32_t sysRead(SysCallRegisters regs)
{
	return 0;
}
int32_t sysKill(SysCallRegisters regs)
{
	return 0;
}
int32_t sysExec(SysCallRegisters regs)
{
	return 0;
}
int32_t sysFstat(SysCallRegisters regs)
{
	return 0;
}
int32_t sysChdir(SysCallRegisters regs)
{
	return 0;
}
int32_t sysDup(SysCallRegisters regs)
{
	return 0;
}
int32_t sysGetpid(SysCallRegisters regs)
{
	return 0;
}
int32_t sysSbrk(SysCallRegisters regs)
{
	return 0;
}
int32_t sysSleep(SysCallRegisters regs)
{
	// Get the milliseconds to sleep for from the ecx
	// register.
	uint32_t ms = regs.ecx;

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
	debug("Sleep complete.");

	return 0;
}
int32_t sysUptime(SysCallRegisters regs)
{
	return 0;
}
int32_t sysOpen(SysCallRegisters regs)
{
	return 0;
}
int32_t sysWrite(SysCallRegisters regs)
{
	return 0;
}
int32_t sysMknod(SysCallRegisters regs)
{
	return 0;
}
int32_t sysUnlink(SysCallRegisters regs)
{
	return 0;
}
int32_t sysLink(SysCallRegisters regs)
{
	return 0;
}
int32_t sysMkdir(SysCallRegisters regs)
{
	return 0;
}
int32_t sysClose(SysCallRegisters regs)
{
	return 0;
}
int32_t sysMalloc(SysCallRegisters regs)
{
	size_t bytes = regs.ecx;
	debug("Allocating %d bytes of memory.", bytes);
	void* ptr = PMM::kmalloc(bytes);
	debug("Out address is %x.", ptr);
	asm("mov %0, %%eax" ::"g"(ptr));
	return 0;
}
int32_t sysFree(SysCallRegisters regs)
{
	// Heap::kfree((void*)regs.ecx);
	return 0;
}
