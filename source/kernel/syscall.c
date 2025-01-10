#include <stdio.h>
#include <syscall.h>
#include <timer.h>

/*
Dispatches the arguments held in registers `ebx`, `ecx`,
 `edx`, `esi`, `edi` to the respective system call. The
 system call code is held in `eax` and is used as an index to
 the `syscalls` array containing a function pointer to each
 system call.
*/
void syscall_dispatcher(registers_t regs)
{
	uint32_t syscall_count = sizeof(syscalls) / sizeof(syscallf_t);

	// System call code is stored in EAX
	if (regs.eax >= syscall_count)
	{
		panic("Invalid syscall %d.", regs.eax);
		return;
	}

	// Arguments for the respective system call function are
	// in order from ebx, ecx, edx, esi, edi.
	syscallf_t syscall = syscalls[regs.eax];
	uint32_t   ret;
	asm("push %1\n"
		"push %2\n"
		"push %3\n"
		"push %4\n"
		"push %5\n"
		"call %6\n"
		"pop %%ebx\n"
		"pop %%ebx\n"
		"pop %%ebx\n"
		"pop %%ebx\n"
		"pop %%ebx\n"
		: "=a"(ret)
		: "r"(regs.edi), "r"(regs.esi), "r"(regs.edx), "r"(regs.ecx), "r"(regs.ebx), "r"(syscall));
}

int32_t sys_fork(syscall_registers_t regs)
{
	return 0;
}
int32_t sys_exit(syscall_registers_t regs)
{
	return 0;
}
int32_t sys_wait(syscall_registers_t regs)
{
	return 0;
}
int32_t sys_pipe(syscall_registers_t regs)
{
	return 0;
}
int32_t sys_read(syscall_registers_t regs)
{
	return 0;
}
int32_t sys_kill(syscall_registers_t regs)
{
	return 0;
}
int32_t sys_exec(syscall_registers_t regs)
{
	return 0;
}
int32_t sys_fstat(syscall_registers_t regs)
{
	return 0;
}
int32_t sys_chdir(syscall_registers_t regs)
{
	return 0;
}
int32_t sys_dup(syscall_registers_t regs)
{
	return 0;
}
int32_t sys_getpid(syscall_registers_t regs)
{
	return 0;
}
int32_t sys_sbrk(syscall_registers_t regs)
{
	return 0;
}
int32_t sys_sleep(syscall_registers_t regs)
{
	uint32_t ms = regs.ecx;

	SLEEP_TICK = ms;
	uint32_t seconds = 0;
	while (SLEEP_TICK)
	{
		if (SLEEP_TICK % PIT_FREQ == 0)
		{
			seconds++;
		}
		asm("sti\nhlt\ncli");
	}
	debug("Sleep complete.");
	return 0;
}
int32_t sys_uptime(syscall_registers_t regs)
{
	return 0;
}
int32_t sys_open(syscall_registers_t regs)
{
	return 0;
}
int32_t sys_write(syscall_registers_t regs)
{
	return 0;
}
int32_t sys_mknod(syscall_registers_t regs)
{
	return 0;
}
int32_t sys_unlink(syscall_registers_t regs)
{
	return 0;
}
int32_t sys_link(syscall_registers_t regs)
{
	return 0;
}
int32_t sys_mkdir(syscall_registers_t regs)
{
	return 0;
}
int32_t sys_close(syscall_registers_t regs)
{
	return 0;
}
