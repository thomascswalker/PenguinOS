#include <stdio.h>
#include <syscall.h>

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
	uint32_t ms = regs.ebx / 4;
	warning("Sleep for %dms.", ms);
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

void syscall_handler(registers_t regs)
{
	uint32_t syscall_count = sizeof(syscalls) / sizeof(syscallf_t);
	if (regs.eax >= syscall_count)
	{
		panic("Invalid syscall %d.", regs.eax);
		return;
	}
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
