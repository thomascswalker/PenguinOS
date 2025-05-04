#include <cstdio.h>
#include <idt.h>
#include <memory.h>
#include <pic.h>
#include <pit.h>
#include <syscall.h>
#include <vfs.h>

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
	sysReadDir,
};

/*
Dispatches the arguments held in registers `ebx`, `ecx`,
 `edx`, `esi`, `edi` to the respective system call. The
 system call code is held in `eax` and is used as an index to
 the `syscalls` array containing a function pointer to each
 system call.
*/
void sysCallDispatcher(CPUState* regs)
{
	uint32_t count = sizeof(syscalls) / sizeof(SysCallFunc);

	// System call code is stored in EAX
	uint32_t sysNo = regs->eax;
	if (sysNo >= count)
	{
		panic("Invalid syscall %d.", sysNo);
		return;
	}

	regs->eax = syscalls[sysNo](regs);
}

int32_t sysSleep(CPUState* regs)
{
	// Get the milliseconds to sleep for from the ecx
	// register.
	uint32_t ms = regs->ebx;

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

	return 0;
}

/**
 * Handles the system call for opening a file.
 *
 * This function is invoked when a process requests to open a file
 * through the system call interface. It retrieves the filename from
 * the CPU state and performs the necessary operations to open the file.

 * @return An integer representing the file descriptor or an error code.
 *         - On success: Returns a valid file descriptor.
 *         - On failure: Returns a negative error code.
 */
int32_t sysOpen(CPUState* regs)
{
	const char* filename = (const char*)regs->ebx;
	auto		vfs = getVirtualFileSystem();
	return vfs->open(filename);
}

int32_t sysRead(CPUState* regs)
{
	int32_t fd = regs->ebx;
	void*	buffer = (void*)regs->ecx;
	size_t	size = regs->edx;

	auto vfs = getVirtualFileSystem();
	return vfs->read(fd, buffer, size);
}

int32_t sysClose(CPUState* regs)
{
	int32_t fd = regs->ebx;
	auto	vfs = getVirtualFileSystem();
	vfs->close(fd);
	return 0;
}

int32_t sysReadDir(CPUState* regs)
{
	// Make a local copy of the filename so we don't modify
	// the original
	Array<File*>* files = (Array<File*>*)regs->ecx;
	files->clear(); // Clear the array before populating it.
	auto	fs = getVirtualFileSystem();
	int32_t fd = regs->ebx;
	debugd(fd);
	*files = fs->getFilesInDirectory(fd);
	return 0;
}

int32_t sysWrite(CPUState* regs) { return 0; }
int32_t sysFstat(CPUState* regs)
{
	int32_t	  fd = regs->ebx;
	FileStat* stat = (FileStat*)regs->ecx;
	auto	  vfs = getVirtualFileSystem();
	stat->size = vfs->getFileSize(fd);
	return 0;
}

int32_t sysFork(CPUState* regs) { return 0; }
int32_t sysExit(CPUState* regs) { return 0; }
int32_t sysWait(CPUState* regs) { return 0; }
int32_t sysPipe(CPUState* regs) { return 0; }
int32_t sysKill(CPUState* regs) { return 0; }
int32_t sysExec(CPUState* regs) { return 0; }
int32_t sysChdir(CPUState* regs) { return 0; }
int32_t sysDup(CPUState* regs) { return 0; }
int32_t sysGetpid(CPUState* regs) { return 0; }
int32_t sysSbrk(CPUState* regs) { return 0; }
int32_t sysUptime(CPUState* regs) { return 0; }
int32_t sysMknod(CPUState* regs) { return 0; }
int32_t sysUnlink(CPUState* regs) { return 0; }
int32_t sysLink(CPUState* regs) { return 0; }
int32_t sysMkdir(CPUState* regs) { return 0; }
