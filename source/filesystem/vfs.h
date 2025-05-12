#pragma once

#include <array.h>
#include <fs.h>
#include <ide.h>

#define MAX_FILENAME 256

/**
 * Represents the types of file systems supported.
 */
enum class FileSystemType
{
	None,
	FAT32,
	NTFS,
	EXT2,
};

class VirtualFileSystem
{
protected:
	using NodeArray = Array<INode>;
	ATADevice* m_device = nullptr;

public:
	VirtualFileSystem() = default;
	virtual ~VirtualFileSystem() = default;

	virtual int32_t open(const char* filename) = 0;
	virtual size_t	read(int32_t fd, void* buffer, size_t size) = 0;
	virtual void	close(int32_t fd) = 0;
	virtual size_t	write(int32_t fd, const void* buffer, size_t size) = 0;

	virtual void getFilesInDirectory(int32_t fd, NodeArray* files) = 0;
	virtual void getFilesInDirectoryFromName(const char* filename, NodeArray* files) = 0;

	virtual size_t		   getFileSize(int32_t fd) = 0;
	virtual size_t		   getFileSizeFromName(const char* filename) = 0;
	virtual FileSystemType getType() const = 0;
	virtual const char*	   getTypeName() const = 0;
};

void			   setVirtualFileSystem(VirtualFileSystem* vfs);
VirtualFileSystem* getVirtualFileSystem();