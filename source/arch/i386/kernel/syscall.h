/*
mov eax, 0      ; Move system call index into EAX
int 0x80        ; Trigger system call defined by value in EAX
*/
#pragma once

#include <idt.h>

#define SYSCALL_FORK 0	  // Fork
#define SYSCALL_EXIT 1	  // Exit
#define SYSCALL_WAIT 2	  // Wait
#define SYSCALL_PIPE 3	  // Pipe
#define SYSCALL_READ 4	  // Read
#define SYSCALL_KILL 5	  // Kill
#define SYSCALL_EXEC 6	  // Exec
#define SYSCALL_FSTAT 7	  // Fstat
#define SYSCALL_CHDIR 8	  // Chdir
#define SYSCALL_DUP 9	  // Dup
#define SYSCALL_GETPID 10 // Getpid
#define SYSCALL_SBRK 11	  // Sbrk
#define SYSCALL_SLEEP 12  // Sleep
#define SYSCALL_UPTIME 13 // Uptime
#define SYSCALL_OPEN 14	  // Open
#define SYSCALL_WRITE 15  // Write
#define SYSCALL_MKNOD 16  // Mknod
#define SYSCALL_UNLINK 17 // Unlink
#define SYSCALL_LINK 18	  // Link
#define SYSCALL_MKDIR 19  // Mkdir
#define SYSCALL_CLOSE 20  // Close

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
