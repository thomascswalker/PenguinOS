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
	sysMalloc,
	sysFree,
};

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
	if (regs.rax >= count)
	{
		panic("Invalid syscall %d.", regs.rax);
		return;
	}

	// Arguments for the respective system call function are
	// in order from ebx, ecx, edx, esi, edi.
	SysCallFunc syscall = syscalls[regs.rax];
	uint32_t	ret;
	asm("push %%rdi\n"		 // Save rdi
		"push %%rsi\n"		 // Save rsi
		"push %%rdx\n"		 // Save rdx
		"push %%rcx\n"		 // Save rcx
		"push %%rbx\n"		 // Save rbx
		"call *%[syscall]\n" // Call the syscall function pointer
		"pop %%rbx\n"		 // Restore rbx
		"pop %%rcx\n"		 // Restore rcx
		"pop %%rdx\n"		 // Restore rdx
		"pop %%rsi\n"		 // Restore rsi
		"pop %%rdi\n"		 // Restore rdi
		"iretq\n"			 // Return from interrupt in long mode
		: "=a"(ret)			 // Return value in RAX
		: [syscall] "r"(syscall), "D"(regs.rdi), "S"(regs.rsi), "d"(regs.rdx), "c"(regs.rcx),
		"b"(regs.rbx)
		: "memory");
}

int32_t sysFork(SysCallRegisters regs) { return 0; }
int32_t sysExit(SysCallRegisters regs) { return 0; }
int32_t sysWait(SysCallRegisters regs) { return 0; }
int32_t sysPipe(SysCallRegisters regs) { return 0; }
int32_t sysRead(SysCallRegisters regs) { return 0; }
int32_t sysKill(SysCallRegisters regs) { return 0; }
int32_t sysExec(SysCallRegisters regs) { return 0; }
int32_t sysFstat(SysCallRegisters regs) { return 0; }
int32_t sysChdir(SysCallRegisters regs) { return 0; }
int32_t sysDup(SysCallRegisters regs) { return 0; }
int32_t sysGetpid(SysCallRegisters regs) { return 0; }
int32_t sysSbrk(SysCallRegisters regs) { return 0; }
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
int32_t sysUptime(SysCallRegisters regs) { return 0; }
int32_t sysOpen(SysCallRegisters regs) { return 0; }
int32_t sysWrite(SysCallRegisters regs) { return 0; }
int32_t sysMknod(SysCallRegisters regs) { return 0; }
int32_t sysUnlink(SysCallRegisters regs) { return 0; }
int32_t sysLink(SysCallRegisters regs) { return 0; }
int32_t sysMkdir(SysCallRegisters regs) { return 0; }
int32_t sysClose(SysCallRegisters regs) { return 0; }
int32_t sysMalloc(SysCallRegisters regs)
{
	size_t bytes = regs.ecx;
	debug("Allocating %d bytes of memory.", bytes);
	// void* ptr = kmalloc(bytes);
	// debug("Out address is %x.", ptr);
	// asm("mov %0, %%edx" ::"r"(ptr));
	return 0;
}
int32_t sysFree(SysCallRegisters regs)
{
	// Heap::kfree((void*)regs.ecx);
	return 0;
}
