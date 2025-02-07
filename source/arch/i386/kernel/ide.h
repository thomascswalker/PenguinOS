#pragma once

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

#define BYTES_PER_SECTOR 512

struct ATADevice;

namespace IDE
{
	void	   init();
	void	   callback(Registers regs);
	ATADevice* getDevice(uint32_t index);
} // namespace IDE

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

	void init(bool inPrimary, bool inMaster);
	void wait4ns() const;
	void waitBusy() const;
	void reset() const;
	void select() const;
	void identify();
	void flush() const;

	bool accessSectors(uint32_t sector, uint32_t count, bool read, void* data);
	bool readSectors(uint32_t sector, uint32_t count, void* data);
	bool writeSectors(uint32_t sector, uint32_t count, void* data);

	// Returns the drive size in bytes.
	uint32_t size() const { return sectorCount * BYTES_PER_SECTOR; }
} __attribute__((packed));

struct BootSector
{
	unsigned char oemIdentifier[9];	  // 0x03
	uint16_t	  bytesPerSector;	  // 0x0B
	uint8_t		  sectorsPerCluster;  // 0x0D
	uint16_t	  reservedSectors;	  // 0x0E
	uint8_t		  numberOfFATs;		  // 0x10
	uint16_t	  rootEntries;		  // 0x11
	uint16_t	  numberOfSectors;	  // 0x13
	uint8_t		  mediaDescriptor;	  // 0x15
	uint16_t	  sectorsPerFAT;	  // 0x16
	uint16_t	  sectorsPerHead;	  // 0x18
	uint16_t	  headsPerCylinder;	  // 0x1A
	uint32_t	  hiddenSectors;	  // 0x1C
	uint32_t	  bigNumberOfSectors; // 0x20
	uint32_t	  bigSectorsPerFAT;	  // 0x24
	uint16_t	  extFlags;			  // 0x28
	uint16_t	  fSVersion;		  // 0x2A
	uint32_t	  rootDirectoryStart; // 0x2C
	uint16_t	  fSInfoSector;		  // 0x30
	uint16_t	  backupBootSector;	  // 0x32

	void init(uint8_t* data);
};
