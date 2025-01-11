/*
mov eax, 0      ; Move system call index into EAX
int 0x80        ; Trigger system call defined by value in EAX
*/
#pragma once

#include <idt.h>

typedef enum
{
	// Standard
	SYSCALL_FORK,	// Fork
	SYSCALL_EXIT,	// Exit
	SYSCALL_WAIT,	// Wait
	SYSCALL_PIPE,	// Pipe
	SYSCALL_READ,	// Read
	SYSCALL_KILL,	// Kill
	SYSCALL_EXEC,	// Exec
	SYSCALL_FSTAT,	// Fstat
	SYSCALL_CHDIR,	// Chdir
	SYSCALL_DUP,	// Dup
	SYSCALL_GETPID, // Getpid
	SYSCALL_SBRK,	// Sbrk
	SYSCALL_SLEEP,	// Sleep
	SYSCALL_UPTIME, // Uptime
	SYSCALL_OPEN,	// Open
	SYSCALL_WRITE,	// Write
	SYSCALL_MKNOD,	// Mknod
	SYSCALL_UNLINK, // Unlink
	SYSCALL_LINK,	// Link
	SYSCALL_MKDIR,	// Mkdir
	SYSCALL_CLOSE,	// Close
	// Non-standard
	SYSCALL_MALLOC, // Malloc
	SYSCALL_FREE,	// Free
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

void	syscall_dispatcher(registers_t regs);
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
int32_t sys_malloc(syscall_registers_t regs);
int32_t sys_free(syscall_registers_t regs);

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
	sys_malloc,
	sys_free,
};
