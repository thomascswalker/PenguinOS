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

void	sysCallDispatcher(Registers* regs);
int32_t sysFork(Registers* regs);
int32_t sysExit(Registers* regs);
int32_t sysWait(Registers* regs);
int32_t sysPipe(Registers* regs);
int32_t sysRead(Registers* regs);
int32_t sysKill(Registers* regs);
int32_t sysExec(Registers* regs);
int32_t sysFstat(Registers* regs);
int32_t sysChdir(Registers* regs);
int32_t sysDup(Registers* regs);
int32_t sysGetpid(Registers* regs);
int32_t sysSbrk(Registers* regs);
int32_t sysSleep(Registers* regs);
int32_t sysUptime(Registers* regs);
int32_t sysOpen(Registers* regs);
int32_t sysWrite(Registers* regs);
int32_t sysMknod(Registers* regs);
int32_t sysUnlink(Registers* regs);
int32_t sysLink(Registers* regs);
int32_t sysMkdir(Registers* regs);
int32_t sysClose(Registers* regs);

// Syscall table
typedef int32_t (*SysCallFunc)(Registers*);
