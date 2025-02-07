#pragma once

#include <sys.h>

#define ATA_PRIMARY 0x00
#define ATA_SECONDARY 0x01

#define ATA_MASTER 0x00
#define ATA_SLAVE 0x01

#define ATA_CMD_IDENTIFY 0xEC

#define ATA_REG_DATA 0x00
#define ATA_REG_ERROR 0x01
#define ATA_REG_FEATURES 0x01
#define ATA_REG_SECCOUNT0 0x02
#define ATA_REG_LBA0 0x03
#define ATA_REG_LBA1 0x04
#define ATA_REG_LBA2 0x05
#define ATA_REG_HDDEVSEL 0x06
#define ATA_REG_COMMAND 0x07
#define ATA_REG_STATUS 0x07
#define ATA_REG_SECCOUNT1 0x08
#define ATA_REG_LBA3 0x09
#define ATA_REG_LBA4 0x0A
#define ATA_REG_LBA5 0x0B
#define ATA_REG_CONTROL 0x0C
#define ATA_REG_ALTSTATUS 0x0C
#define ATA_REG_DEVADDRESS 0x0D

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

struct ATADevice
{
	struct Ports
	{
		uint16_t data;
		uint16_t err;
		uint16_t sectorCount;
		uint16_t lbaLow;
		uint16_t lbaMid;
		uint16_t lbaHigh;
		uint16_t device;
		uint16_t command;
		uint16_t control;
	};

	Ports	 ports;
	bool	 drive;	  // Master=0, Slave=1
	bool	 channel; // Primary=0, Secondary=1
	bool	 lbaExtSupported;
	uint32_t sectorCount; // Size in sectors
	uint16_t serial;
	char	 model[41]; // Drive model string

	void init(bool inPrimary, bool inMaster);
	void wait();
	void reset();
	void select();
	void identify();

	// Returns the drive size in bytes.
	uint32_t size() const { return sectorCount * BYTES_PER_SECTOR; }
} __attribute__((packed));

namespace IDE
{
	void init();
} // namespace IDE