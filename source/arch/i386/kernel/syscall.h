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

struct SysCallRegisters
{
	uint32_t edi; // 1st argument
	uint32_t esi; // 2nd argument
	uint32_t edx; // 3rd argument
	uint32_t ecx; // 4th argument
	uint32_t ebx; // 5th argument
	uint32_t eax; // syscall number/return value
} __attribute__((packed));

void	sysCallDispatcher(Registers regs);
int32_t sysFork(SysCallRegisters* regs);
int32_t sysExit(SysCallRegisters* regs);
int32_t sysWait(SysCallRegisters* regs);
int32_t sysPipe(SysCallRegisters* regs);
int32_t sysRead(SysCallRegisters* regs);
int32_t sysKill(SysCallRegisters* regs);
int32_t sysExec(SysCallRegisters* regs);
int32_t sysFstat(SysCallRegisters* regs);
int32_t sysChdir(SysCallRegisters* regs);
int32_t sysDup(SysCallRegisters* regs);
int32_t sysGetpid(SysCallRegisters* regs);
int32_t sysSbrk(SysCallRegisters* regs);
int32_t sysSleep(SysCallRegisters* regs);
int32_t sysUptime(SysCallRegisters* regs);
int32_t sysOpen(SysCallRegisters* regs);
int32_t sysWrite(SysCallRegisters* regs);
int32_t sysMknod(SysCallRegisters* regs);
int32_t sysUnlink(SysCallRegisters* regs);
int32_t sysLink(SysCallRegisters* regs);
int32_t sysMkdir(SysCallRegisters* regs);
int32_t sysClose(SysCallRegisters* regs);

// Syscall table
typedef int32_t (*SysCallFunc)(SysCallRegisters*);
