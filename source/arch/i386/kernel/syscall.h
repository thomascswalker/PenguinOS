/*
mov eax, 0      ; Move system call index into EAX
int 0x80        ; Trigger system call defined by value in EAX
*/
#pragma once

#include <idt.h>

enum SysCalls
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
};

void	sysCallDispatcher(CPUState* regs);
int32_t sysFork(CPUState* regs);
int32_t sysExit(CPUState* regs);
int32_t sysWait(CPUState* regs);
int32_t sysPipe(CPUState* regs);
int32_t sysRead(CPUState* regs);
int32_t sysKill(CPUState* regs);
int32_t sysExec(CPUState* regs);
int32_t sysFstat(CPUState* regs);
int32_t sysChdir(CPUState* regs);
int32_t sysDup(CPUState* regs);
int32_t sysGetpid(CPUState* regs);
int32_t sysSbrk(CPUState* regs);
int32_t sysSleep(CPUState* regs);
int32_t sysUptime(CPUState* regs);
int32_t sysOpen(CPUState* regs);
int32_t sysWrite(CPUState* regs);
int32_t sysMknod(CPUState* regs);
int32_t sysUnlink(CPUState* regs);
int32_t sysLink(CPUState* regs);
int32_t sysMkdir(CPUState* regs);
int32_t sysClose(CPUState* regs);

// Syscall table
typedef int32_t (*SysCallFunc)(CPUState*);
