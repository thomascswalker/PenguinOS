#pragma once

#include <array.h>
#include <ide.h>
#include <string.h>

/**
 * Represents the types of file systems supported.
 */
enum class FileSystemType
{
	None,
	FAT32,
	NTFS,
};

class VirtualFileSystem
{
protected:
	ATADevice* m_device;

public:
	VirtualFileSystem() = default;
	virtual ~VirtualFileSystem() = default;

	virtual int32_t open(const char* filename) = 0;
	virtual size_t	read(int32_t fd, void* buffer, size_t size) = 0;
	virtual void	close(int32_t fd) = 0;

	virtual Array<File*> getFilesInDirectory(int32_t fd) = 0;
	virtual Array<File*> getFilesInDirectoryFromName(const char* filename) = 0;

	virtual size_t		   getFileSize(int32_t fd) = 0;
	virtual size_t		   getFileSizeFromName(const char* filename) = 0;
	virtual FileSystemType getType() const = 0;
	virtual String		   getTypeName() const = 0;
};

void			   setVirtualFileSystem(VirtualFileSystem* vfs);
VirtualFileSystem* getVirtualFileSystem();