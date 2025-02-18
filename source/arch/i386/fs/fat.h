// https://academy.cba.mit.edu/classes/networking_communications/SD/FAT.pdf

#pragma once

#include <bitmask.h>
#include <cstring.h>
#include <ide.h>
#include <string.h>
#include <sys.h>

#define MBR_BYTE_SIZE 446
#define FAT_BYTES_PER_SECTOR 512
#define FAT_ENTRY_SIZE 32
#define FAT_ENTRIES_PER_SECTOR (FAT_BYTES_PER_SECTOR / FAT_ENTRY_SIZE)
#define MAX_FILENAME 256
#define FAT_EOC 0x0FFFFFF8

enum class FATType
{
	ExFAT,
	FAT12,
	FAT16,
	FAT32
};

enum FATEntryType
{
	Free,
	Allocated,
	Reserved,
	Defective,
	EOF
};

struct FATShortEntry // 32 bytes
{
	uint8_t	 name[8];
	uint8_t	 ext[3];
	uint8_t	 attr;
	uint8_t	 ntRes;
	uint8_t	 crtTimeTenth;
	uint16_t crtTime;
	uint16_t crtDate;
	uint16_t lastAccDate;
	uint16_t firstClusterHigh;
	uint16_t wrtTime;
	uint16_t wrtDate;
	uint16_t firstClusterLow;
	uint32_t fileSize = 0;

	bool	 isValid() const { return name[0] != 0x0 && name[0] != 0xE5; }
	uint32_t firstCluster() const { return firstClusterLow | (firstClusterHigh << 16); }
} __attribute__((packed));

struct FATLongEntry // 32 bytes
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

enum FileAttribute
{
	FA_Empty = 0,
	FA_ReadOnly = (1 << 0),
	FA_Hidden = (1 << 1),
	FA_System = (1 << 2),
	FA_VolumeID = (1 << 3),
	FA_LongFileName = 0x0F,
	FA_Directory = (1 << 4),
	FA_Archive = (1 << 5),
	FA_LastEntry = 0x41,
	FA_Deleted = 0xE5,
};

struct FATEntry
{
	size_t	 size;
	char	 name[64];
	char	 ext[3];
	uint16_t sector;
	uint8_t	 attr;
};

namespace FAT32
{
	String		 toShortName(const String& longName);
	String		 sanitize(const String& longName);
	bool		 isValidChar(char c);
	uint32_t	 getNextCluster(uint32_t cluster);
	uint32_t	 getClusterSector(uint32_t n);
	uint32_t	 getSector(uint32_t FATNumber, uint32_t FATSize, uint32_t FATSector);
	FATEntryType getEntryType(uint32_t index);
	uint32_t	 getClusterCount();
	uint32_t	 getSize();
	bool		 isLongEntry(uint8_t* buffer);
	void		 parseLongEntry(FATLongEntry* entry, uint32_t count, char* filename);
	bool		 findEntry(uint32_t startCluster, const String& name, FATShortEntry* entry);
	bool		 readFile(const String& filename, void* file);

} // namespace FAT32