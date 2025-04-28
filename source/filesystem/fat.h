// https://academy.cba.mit.edu/classes/networking_communications/SD/FAT.pdf

#pragma once

#include <array.h>
#include <bitmask.h>
#include <cstring.h>
#include <ide.h>
#include <pair.h>
#include <string.h>
#include <sys.h>
#include <vfs.h>

// Maximum number of characters in a file name
#define MAX_FILENAME 256
// Byte size of the MBR (Boot Sector)
#define MBR_BYTE_SIZE 446
// Byte size of a FAT entry (short or long)
#define FAT_ENTRY_SIZE 32
#define FAT_END_OF_CLUSTER 0x0FFFFFF8
#define FAT_ENTRIES_PER_SECTOR 16

namespace FAT32
{
	enum class Type
	{
		ExFAT,
		FAT12,
		FAT16,
		FAT32
	};

	enum class EntryType
	{
		Free,
		Allocated,
		Reserved,
		Defective,
		EOF
	};

	enum Attribute : uint8_t
	{
		FA_Empty = 0,
		FA_ReadOnly = 0x01,
		FA_Hidden = 0x02,
		FA_System = 0x04,
		FA_VolumeID = 0x08,
		FA_LongFileName = FA_ReadOnly | FA_Hidden | FA_System | FA_VolumeID,
		FA_Directory = 0x10,
		FA_Archive = 0x20,
		FA_LastEntry = 0x41,
		FA_Deleted = 0xE5,
	};

	struct ShortEntry // 32 bytes
	{
		uint8_t	  name[8];
		uint8_t	  ext[3];
		Attribute attribute;
		uint8_t	  ntRes;
		uint8_t	  creationTimeSeconds;
		uint16_t  creationTime;
		uint16_t  creationDate;
		uint16_t  lastAccDate;
		uint16_t  clusterHigh;
		uint16_t  writeTime;
		uint16_t  writeDate;
		uint16_t  clusterLow;
		uint32_t  fileSize = 0;

		bool hasExtension() const { return ext[0] != 0 && ext[0] != ' '; }
		bool isValid() const { return name[0] != FA_Empty && name[0] != FA_Deleted; }
		// Returns the full 32-bit cluster number composed of the low 16 bits
		// of `clusterLow` and high 16 bits of `clusterHigh`.
		uint32_t cluster() const { return clusterLow | (clusterHigh << 16); }
	} __attribute__((packed));

	struct LongEntry // 32 bytes
	{
		uint8_t	 id;
		uint8_t	 data0[10];
		uint8_t	 attr; // Should always be 0x0F
		uint8_t	 longEntryType;
		uint8_t	 checksum;
		uint8_t	 data1[12];
		uint16_t always0;
		uint8_t	 data2[4];

		bool isLast() const { return (id & 0x40) == 0x40; }

	} __attribute__((packed));

	struct BaseEntry
	{
		ShortEntry* shortEntry;
		char*		filename;
	} __attribute__((packed));

} // namespace FAT32

class FAT32FileSystem : public VirtualFileSystem
{
	typedef Pair<int32_t, FAT32::ShortEntry> EntryType;

	FAT32::ShortEntry m_rootEntry;
	Array<EntryType>  m_openEntries;

public:
	FAT32FileSystem();
	~FAT32FileSystem() override = default;

	int32_t open(const char* filename) override;
	size_t	read(int32_t fd, void* buffer, size_t size) override;
	void	close(int32_t fd) override;

	Array<File*> getFilesInDirectory(int32_t fd) override;
	Array<File*> getFilesInDirectoryFromName(const char* filename) override;

	size_t		   getFileSize(int32_t fd) override;
	size_t		   getFileSizeFromName(const char* filename) override;
	FileSystemType getType() const override { return FileSystemType::FAT32; };
	String		   getTypeName() const override { return "FAT32"; };

	bool	 getEntryFromPath(const char* filename, FAT32::ShortEntry* entry);
	uint32_t getNextCluster(uint32_t n);
	uint32_t getClusterSector(uint32_t n);
	uint32_t getClusterCount();
	uint32_t getSize();

	bool findEntry(uint32_t startCluster, const String& name, FAT32::ShortEntry* entry);
	bool readDirectory(const FAT32::ShortEntry& entry, Array<FAT32::ShortEntry>& entries);

	char* toShortName(const char* longName);
	char* sanitize(const String& component, size_t count);
	bool  isValidChar(char c);
	bool  isLongEntry(uint8_t* buffer);

	char* parseLongEntryName(FAT32::LongEntry* entry, uint32_t count);
	char* parseShortEntryName(FAT32::ShortEntry* entry);
};
