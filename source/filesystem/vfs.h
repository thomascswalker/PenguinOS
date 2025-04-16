#pragma once

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

	virtual FileSystemType getType() const = 0;
	virtual String		   getTypeName() const = 0;
};