/*
mov eax, 0      ; Move system call index into EAX
int 0x80        ; Trigger system call defined by value in EAX
*/
#pragma once

#include <idt.h>

typedef enum
{
	SYSCALL_FORK,
	SYSCALL_EXIT,
	SYSCALL_WAIT,
	SYSCALL_PIPE,
	SYSCALL_READ,
	SYSCALL_KILL,
	SYSCALL_EXEC,
	SYSCALL_FSTAT,
	SYSCALL_CHDIR,
	SYSCALL_DUP,
	SYSCALL_GETPID,
	SYSCALL_SBRK,
	SYSCALL_SLEEP,
	SYSCALL_UPTIME,
	SYSCALL_OPEN,
	SYSCALL_WRITE,
	SYSCALL_MKNOD,
	SYSCALL_UNLINK,
	SYSCALL_LINK,
	SYSCALL_MKDIR,
	SYSCALL_CLOSE
} syscall_t;

__attribute__((packed)) struct syscall_registers
{
	uint32_t esp;
	uint32_t ebx;
	uint32_t ecx;
	uint32_t edx;
	uint32_t esi;
	uint32_t edi;
	uint32_t ebp;
	uint32_t ds;
	uint32_t es;
	uint32_t fs;
	uint32_t gs;
	uint32_t eax;
};
typedef struct syscall_registers syscall_registers_t;

int32_t sys_fork(syscall_registers_t regs);
int32_t sys_exit(syscall_registers_t regs);
int32_t sys_wait(syscall_registers_t regs);
int32_t sys_pipe(syscall_registers_t regs);
int32_t sys_read(syscall_registers_t regs);
int32_t sys_kill(syscall_registers_t regs);
int32_t sys_exec(syscall_registers_t regs);
int32_t sys_fstat(syscall_registers_t regs);
int32_t sys_chdir(syscall_registers_t regs);
int32_t sys_dup(syscall_registers_t regs);
int32_t sys_getpid(syscall_registers_t regs);
int32_t sys_sbrk(syscall_registers_t regs);
int32_t sys_sleep(syscall_registers_t regs);
int32_t sys_uptime(syscall_registers_t regs);
int32_t sys_open(syscall_registers_t regs);
int32_t sys_write(syscall_registers_t regs);
int32_t sys_mknod(syscall_registers_t regs);
int32_t sys_unlink(syscall_registers_t regs);
int32_t sys_link(syscall_registers_t regs);
int32_t sys_mkdir(syscall_registers_t regs);
int32_t sys_close(syscall_registers_t regs);

// Syscall table
typedef int32_t (*syscallf_t)(syscall_registers_t);
static syscallf_t syscalls[] = {
	sys_fork,
	sys_exit,
	sys_wait,
	sys_pipe,
	sys_read,
	sys_kill,
	sys_exec,
	sys_fstat,
	sys_chdir,
	sys_dup,
	sys_getpid,
	sys_sbrk,
	sys_sleep,
	sys_uptime,
	sys_open,
	sys_write,
	sys_mknod,
	sys_unlink,
	sys_link,
	sys_mkdir,
	sys_close,
};

void syscall_handler(registers_t regs);