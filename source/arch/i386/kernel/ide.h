// https://academy.cba.mit.edu/classes/networking_communications/SD/FAT.pdf

#pragma once

#include <bitmask.h>
#include <cstring.h>
#include <sys.h>

#define ATA_PRIMARY 0x00
#define ATA_SECONDARY 0x01

#define ATA_MASTER 0x00
#define ATA_SLAVE 0x01

#define ATA_CMD_READ 0x20
#define ATA_CMD_WRITE 0x30
#define ATA_CMD_IDENTIFY 0xEC

// https://wiki.osdev.org/ATA_Command_Matrix
// https://people.freebsd.org/~imp/asiabsdcon2015/works/d2161r5-ATAATAPI_Command_Set_-_3.pdf
// Page 104
// Device type
#define ATA_IDENT_DEVICETYPE 0
// #define ATA_IDENT_CYLINDERS 1 // Obsolete
// #define ATA_IDENT_HEADS 3 // Obsolete
// #define ATA_IDENT_SECTORS 6 // Obsolete
// Serial number
#define ATA_IDENT_SERIAL 10
// Model number
#define ATA_IDENT_MODEL 27
#define ATA_IDENT_CAPABILITIES 49
#define ATA_IDENT_FIELDVALID 53
// Total number of user addressable logical sectors for
// 28-bit commands (DWord, 32)
#define ATA_IDENT_MAX_LBA 60
#define ATA_IDENT_COMMANDSETS 82
// Number of User Addressable Logical Sectors (QWord, 64)
#define ATA_IDENT_MAX_LBA_EXT 100

#define MBR_BYTE_SIZE 446
#define FAT_BYTES_PER_SECTOR 512
#define FAT_ENTRY_SIZE 32
#define FAT_ENTRIES_PER_SECTOR (FAT_BYTES_PER_SECTOR / FAT_ENTRY_SIZE)

struct ATADevice;
struct Partition;
struct BootSector;

namespace IDE
{
	void	   init();
	void	   callback(Registers regs);
	ATADevice* getDevice(uint32_t index);
} // namespace IDE

enum FAT
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

struct Partition
{
	uint32_t sectorCount;
	uint32_t lbaBegin;
	uint32_t chsEnd;
	uint8_t	 typeCode;
	uint32_t chsBegin;
	uint8_t	 bootFlag;
	FAT		 fatType;
} __attribute__((packed));

struct BootSector
{
	unsigned char oemIdentifier[9];	   // 0x03
	uint16_t	  bytesPerSector;	   // 0x0B
	uint8_t		  sectorsPerCluster;   // 0x0D
	uint16_t	  reservedSectorCount; // 0x0E, location of FAT table as a sector number
	uint8_t		  tableCount;		   // 0x10
	uint16_t	  rootEntryCount;	   // 0x11
	uint16_t	  sectorCount;		   // 0x13
	uint8_t		  mediaType;		   // 0x15
	uint16_t	  sectorsPerTable;	   // 0x16, FAT12/FAT16
	uint16_t	  sectorsPerTrack;	   // 0x18
	uint16_t	  heads;			   // 0x1A
	uint32_t	  hiddenSectors;	   // 0x1C
	uint32_t	  largeSectorCount;	   // 0x20
	uint32_t	  bigSectorsPerTable;  // 0x24, FAT32
	uint16_t	  extFlags;			   // 0x28
	uint16_t	  fSVersion;		   // 0x2A
	uint32_t	  rootDirectoryStart;  // 0x2C
	uint16_t	  fSInfoSector;		   // 0x30
	uint16_t	  backupBootSector;	   // 0x32
} __attribute__((packed));

struct FileSystemInfo
{
	uint32_t signature0;
	uint8_t	 reserved1[480];
	uint32_t signature1;
	uint32_t freeCount;
	uint32_t nxtFree;
	uint8_t	 reserved2[12];
	uint32_t signature2;
} __attribute__((packed));

struct FATFileEntry // 32 bytes
{
	uint8_t	 shortName[8];
	uint8_t	 ext[3];
	uint8_t	 attr;
	uint8_t	 ntRes;
	uint8_t	 crtTimeTenth;
	uint16_t crtTime;
	uint16_t crtDate;
	uint16_t lastAccDate;
	uint16_t fstClusHI;
	uint16_t wrtTime;
	uint16_t wrtDate;
	uint16_t fstClusLO;
	uint32_t fileSize;
} __attribute__((packed));

struct FATLongFileEntry // 32 bytes
{
	uint8_t	 id;
	char	 data0[10];
	uint8_t	 attr; // Should always be 0x0F
	uint8_t	 longEntryType;
	uint8_t	 checksum;
	char	 data1[12];
	uint16_t always0;
	char	 data2[4];

	bool isLast() const { return (id & 0x40) == 0x40; }

} __attribute__((packed));

enum FileAttribute
{
	ReadOnly = (1 << 0),
	Hidden = (1 << 1),
	System = (1 << 2),
	VolumeID = (1 << 3),
	Directory = (1 << 4),
	Archive = (1 << 5),
};

struct ATADevice
{
	struct Ports
	{
		uint16_t data;		  // 0x1F0 | 0x170
		uint16_t err;		  // 0x1F1 | 0x171
		uint16_t sectorCount; // 0x1F2 | 0x172
		uint16_t lbaLow;	  // 0x1F3 | 0x173
		uint16_t lbaMid;	  // 0x1F4 | 0x174
		uint16_t lbaHigh;	  // 0x1F5 | 0x175
		uint16_t device;	  // 0x1F6 | 0x176
		uint16_t command;	  // 0x1F7 | 0x177
		uint16_t control;	  // 0x3F6 | 0x376
	};

	Ports	 ports;
	bool	 drive;	  // Master=0, Slave=1
	bool	 channel; // Primary=0, Secondary=1
	bool	 lbaExtSupported;
	uint32_t sectorCount; // Size in sectors
	uint16_t serial;
	char	 model[41]; // Drive model string, 40 chars + null terminator

	uint8_t		   bootCode[446];
	BootSector	   mbr;
	Partition	   partitions[4];
	FileSystemInfo fsi;

	uint32_t	 rootDirectorySector;
	uint32_t	 firstDataSector;
	FATFileEntry rootDirectory;

	void init(bool inPrimary, bool inMaster);
	void wait4ns() const;
	void waitBusy() const;
	void reset() const;
	void select() const;
	void identify();
	void flush() const;

	void parseBootSector();
	void parseFileSystemInfoSector();
	void parseDirectory(uint32_t sector, bool recurse);

	bool isLongEntry(uint8_t* buffer);
	void parseEntry(FATFileEntry* entry, char* buffer, uint32_t* pos);
	void parseLongEntry(FATLongFileEntry* entry, uint32_t count, char* filename);

	uint32_t getClusterSector(uint32_t n);

	bool accessSectors(uint32_t sector, uint32_t count, bool read, void* data);
	bool readSectors(uint32_t sector, uint32_t count, void* data);
	bool writeSectors(uint32_t sector, uint32_t count, void* data);

	uint32_t	 getFATSector(uint32_t FATNumber, uint32_t FATSize, uint32_t FATSector);
	FATEntryType getFATEntry(uint32_t index);
	uint32_t	 getClusterCount();
	uint32_t	 getFATSize();

	// Returns the drive size in bytes.
	uint32_t size() const { return sectorCount * FAT_BYTES_PER_SECTOR; }
} __attribute__((packed));
