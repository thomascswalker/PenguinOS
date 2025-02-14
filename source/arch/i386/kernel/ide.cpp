#include <assert.h>
#include <bitmask.h>
#include <ide.h>
#include <idt.h>
#include <math.h>
#include <memory.h>
#include <stdio.h>

ATADevice devices[4];

void IDE::init()
{
	info("Initializing IDE/ATA...");
	devices[0].init(true, true);
	IDT::registerInterruptHandler(IRQ14, IDE::callback);
}

void IDE::callback(Registers regs) { /*debug("File IO callback");*/ }

ATADevice* IDE::getDevice(uint32_t index) { return &devices[index]; }

void ATADevice::init(bool inPrimary, bool inMaster)
{
	drive = inMaster;
	channel = inPrimary;
	uint16_t base = channel ? 0x1F0 : 0x170;

	ports.data = base;
	ports.err = base + 1;
	ports.sectorCount = base + 2;
	ports.lbaLow = base + 3;
	ports.lbaMid = base + 4;
	ports.lbaHigh = base + 5;
	ports.device = base + 6;
	ports.command = base + 7;
	ports.control = channel ? 0x3F6 : 0x376;

	identify();

	// Parse the boot sector (1)
	parseBootSector();

	// Parse the file system info sector (2)
	parseFileSystemInfoSector();

	// Parse the root directory
	FATShortEntry data[FAT_ENTRIES_PER_SECTOR];
	if (!readSectors(rootDirectorySector, 1, &data))
	{
		panic("Failed to read root directory sector.");
	}
	rootDirectory = data[0];

	Directory directory;
	memset(directory.files, 0, sizeof(FATEntry) * FAT_ENTRIES_PER_SECTOR);
	uint32_t rootDataSector = getClusterSector(rootDirectory.fstClusLO);
	parseDirectory(rootDataSector, &directory);
	for (int32_t i = 0; i < directory.fileCount; i++)
	{
		FATEntry* f = &directory.files[i];

		// Skip directories
		if (std::Bitmask::test(f->attr, FA_Directory))
		{
			continue;
		}
		char* data = (char*)std::kmalloc(f->size);
		readSectors(f->sector, CEILDIV(1, mbr.bytesPerSector), data);
		printf("%s, %dB:\n%s\n\n", f->name, f->size, data);
	}
}

void ATADevice::wait4ns() const
{
	for (uint32_t i = 0; i < 4; i++)
	{
		inb(ports.control);
	}
}

void ATADevice::reset() const
{
	outb(ports.control, 0x4);
	wait4ns();
	outb(ports.control, 0x0);
}

// Wait until BSY bit is clear
void ATADevice::waitBusy() const
{
	uint8_t mask = (1 << 7);
	while (inb(ports.command) & mask)
	{
	}
}

void ATADevice::select() const
{
	if (drive)
	{
		outb(ports.device, 0xA0);
	}
	else
	{
		outb(ports.device, 0xB0);
	}
}

void ATADevice::identify()
{
	reset();
	select();

	wait4ns();

	outb(ports.sectorCount, 0);
	outb(ports.lbaLow, 0);
	outb(ports.lbaMid, 0);
	outb(ports.lbaHigh, 0);

	outb(ports.command, ATA_CMD_IDENTIFY);

	uint8_t status = inb(ports.command);
	if (status != 0x58)
	{
		return;
	}

	waitBusy();

	// Read 256 WORD (2-byte) values (512 bytes total).
	uint16_t buffer[256];
	memset(buffer, 0, 256);
	for (int i = 0; i < 256; i++)
	{
		buffer[i] = inw(ports.data);
	}

	// Bit 15 of the first word of the IDENTIFY buffer
	// determines whether the device is a valid ATA device.
	if ((buffer[0] & 0x40) != 0x40)
	{
		panic("ATA Device not recognized.");
	}

	// 0b000_0100_0000_0000
	lbaExtSupported = (buffer[83] & 0x400) == 0x400;
	if (lbaExtSupported)
	{
		println("48-bit Address");
	}
	else
	{
		println("28-bit Address");
	}

	serial = buffer[ATA_IDENT_SERIAL];
	printf("Serial: %d\n", serial);

	sectorCount = *((uint32_t*)(buffer + ATA_IDENT_MAX_LBA));
	printf("Sector count: %d\n", sectorCount);
	printf("Drive size: %dMB\n", size() / 1024 / 1024);

	uint8_t* modelPtr = (uint8_t*)(buffer + ATA_IDENT_MODEL);
	/*
		ATA strings are weird and are written in 16bit increments,
		where the second 8bits are the first character and the
		first 8bits are the second character.

		-------------------------
		| Char 1 | Char 3 | ... |
		| Char 0 | Char 2 | ... |
		-------------------------
	*/

	for (uint32_t i = 0; i < 21; i++)
	{
		auto offset = i * 2;
		model[offset] = modelPtr[offset + 1];
		model[offset + 1] = modelPtr[offset];
	}
	model[40] = 0; // Terminate string at the last byte
	printf("Drive model: %s\n", model);
}

void ATADevice::flush() const { outb(ports.command, 0xE7); }

void ATADevice::parseBootSector()
{
	// Read the boot sector
	// https://averstak.tripod.com/fatdox/bootsec.htm
	uint8_t mbrData[512];
	if (!readSectors(0, 1, mbrData))
	{
		panic("Failed to read boot sector of drive data.");
	}

	// https://www.pjrc.com/tech/8051/ide/fat32.html
	// Extract MBR info
	memcpy(bootCode, mbrData, MBR_BYTE_SIZE);

#define BOOT_SECTOR(x, y) memcpy(&x, mbrData + y, sizeof(x));

	memcpy(&mbr.oemIdentifier, (uint8_t*)mbrData + 0x03, 8);
	mbr.oemIdentifier[8] = 0; // Null-terminate string
	BOOT_SECTOR(mbr.bytesPerSector, 0x0B);
	BOOT_SECTOR(mbr.sectorsPerCluster, 0x0D);
	BOOT_SECTOR(mbr.reservedSectorCount, 0x0E);
	BOOT_SECTOR(mbr.tableCount, 0x10);
	BOOT_SECTOR(mbr.rootEntryCount, 0x11);
	BOOT_SECTOR(mbr.sectorCount, 0x13);
	BOOT_SECTOR(mbr.mediaType, 0x15);
	BOOT_SECTOR(mbr.sectorsPerTable, 0x16);
	BOOT_SECTOR(mbr.sectorsPerTrack, 0x18);
	BOOT_SECTOR(mbr.heads, 0x1A);
	BOOT_SECTOR(mbr.hiddenSectors, 0x1C);
	BOOT_SECTOR(mbr.largeSectorCount, 0x20);
	BOOT_SECTOR(mbr.bigSectorsPerTable, 0x24);
	BOOT_SECTOR(mbr.extFlags, 0x28);
	BOOT_SECTOR(mbr.fSVersion, 0x2A);
	BOOT_SECTOR(mbr.rootDirectoryStart, 0x2C);
	BOOT_SECTOR(mbr.fSInfoSector, 0x30);
	BOOT_SECTOR(mbr.backupBootSector, 0x32);
#undef BOOT_SECTOR

	// Extract partition info
	uint32_t partitionSize = 64;
	memcpy(&partitions, mbrData + MBR_BYTE_SIZE, partitionSize);

	// The last two bytes of the MBR should ALWAYS be
	// [0x55, 0xAA]
	uint8_t lastTwoBytes[2];
	memcpy(&lastTwoBytes, mbrData + MBR_BYTE_SIZE + partitionSize, 2);
	ASSERT(lastTwoBytes[0] == 0x55);
	ASSERT(lastTwoBytes[1] == 0xAA);

	rootDirectorySector = mbr.reservedSectorCount + (mbr.tableCount * mbr.bigSectorsPerTable);
	firstDataSector = mbr.reservedSectorCount + (mbr.tableCount * mbr.bigSectorsPerTable);

	uint32_t rootDirSectors =
		((mbr.rootEntryCount * 32) + (mbr.bytesPerSector - 1)) / mbr.bytesPerSector;
	ASSERT(rootDirSectors == 0); // Should always be 0 on FAT32
}

void ATADevice::parseFileSystemInfoSector()
{
	if (!readSectors(1, 1, &fsi))
	{
		panic("Failed to read file system info sector.");
	}

	// Verify integrity of the FSI structure
	ASSERT(fsi.signature0 == 0x41615252);
	ASSERT(fsi.signature1 == 0x61417272);
	ASSERT(fsi.signature2 == 0xAA550000);
}

void ATADevice::parseDirectory(uint32_t sector, Directory* directory)
{
	if (!sector || sector >= mbr.sectorCount)
	{
		error("Sector %d is invalid.", sector);
		return;
	}

	uint8_t data[512];
	if (!readSector(sector, data))
	{
		panic("Failed to read root directory sector.");
	}

	FATEntry* file = directory->files;
	uint32_t  index = 0;
	while (index < FAT_ENTRIES_PER_SECTOR)
	{
		uint8_t* attr = &data[index * FAT_ENTRY_SIZE];
		char	 longName[MAX_FILENAME];
		memset(longName, 0, MAX_FILENAME);
		bool isLong = isLongEntry(attr);

		if (isLong)
		{
			FATLongEntry* entry = (FATLongEntry*)(data + (index * FAT_ENTRY_SIZE));
			uint8_t		  entryCount = (entry->id & ~0x40);
			parseLongEntry(entry, entryCount, longName);
			index += entryCount;
		}
		attr = &data[index * FAT_ENTRY_SIZE];
		if (*attr != 0x0)
		{
			FATShortEntry* entry = (FATShortEntry*)(data + (index * FAT_ENTRY_SIZE));
			char		   shortName[13];
			for (int i = 0; i < 8; i++)
			{
				shortName[i] = tolower(entry->shortName[i]);
			}
			if (!isLong && std::Bitmask::test(*attr, FA_Directory))
			{
				shortName[8] = '.';
				for (int i = 0; i < 3; i++)
				{
					shortName[i + 9] = tolower(entry->ext[i]);
				}
				shortName[12] = 0;
			}
			else
			{
				shortName[8] = 0;
			}

			char* nameBuffer = longName[0] ? longName : shortName;
			memcpy(file->name, nameBuffer, strlen(nameBuffer));
			file->size = entry->fileSize;
			memcpy(file->ext, entry->ext, 3);
			file->sector = getClusterSector(entry->fstClusLO);
			file->attr = entry->attr;

			directory->fileCount++;
			file++;
		}
		index++;
	}
}

bool ATADevice::getEntry(const char* name, FATEntry* entry) { return false; }

bool ATADevice::isLongEntry(uint8_t* buffer)
{
	return *buffer == FA_LastEntry || *(buffer + 11) == FA_LongFileName;
}

void ATADevice::parseLongEntry(FATLongEntry* entry, uint32_t count, char* filename)
{
	while (count)
	{
		count--;
		uint32_t offset = count * 13;
		char	 wdata[26];
		memcpy(wdata, entry->data0, 10);
		memcpy(wdata + 10, entry->data1, 12);
		memcpy(wdata + 22, entry->data2, 4);
		for (int i = 0; i < 13; i++)
		{
			*(filename + offset + i) = wdata[i * 2];
		}
		entry++;
	}
}

bool ATADevice::readFile(uint32_t startCluster, char* data, uint32_t size)
{
	uint32_t bytesRead = 0;
	uint32_t cluster = startCluster;
	while (cluster < FAT_EOC && bytesRead < size)
	{
		uint32_t sector = getClusterSector(cluster);
		uint8_t	 buffer[512];
		readSector(sector, buffer);
		uint32_t copySize = mbr.bytesPerSector;
		if (bytesRead + copySize > size)
		{
			copySize = size - bytesRead;
		}
		memcpy(data + bytesRead, buffer, copySize);
		bytesRead += copySize;
	}
	return true;
}

uint32_t ATADevice::getClusterSector(uint32_t n)
{
	return ((n - 2) * mbr.sectorsPerCluster) + firstDataSector;
}

bool ATADevice::accessSectors(uint32_t sector, uint32_t count, bool read, void* data)
{
	if (count == 0)
	{
		warning("Unable to read 0 sectors.");
		return false;
	}
	outb(ports.device, (0xE0 | ((sector >> 24) & 0x0F))); // Head/drive
	outb(ports.sectorCount, count);				  // Sector count port - # of sectors to read/write
	outb(ports.lbaLow, sector & 0xFF);			  // Sector number port / LBA low bits 0-7
	outb(ports.lbaMid, ((sector >> 8) & 0xFF));	  // Cylinder low port / LBA mid bits 8-15
	outb(ports.lbaHigh, ((sector >> 16) & 0xFF)); // Cylinder high port / LBA high bits 16-23

	uint8_t command = read ? ATA_CMD_READ : ATA_CMD_WRITE;
	outb(ports.command, command); // Command port - send read/write command

	uint16_t* ptr = (uint16_t*)(data);

	// Read
	if (read)
	{
		for (uint32_t i = count; i > 0; i--)
		{
			// debug("Reading sector %d.", sector);
			waitBusy();
			// Read 256 words
			for (uint32_t j = 0; j < 256; j++)
			{
				*ptr++ = inw(ports.data);
			}
			wait4ns();
		}
	}
	else
	{
		for (uint32_t i = count; i > 0; i--)
		{
			debug("Writing sector %d.", sector);
			waitBusy();
			// Write 256 words
			for (uint32_t j = 0; j < 256; j++)
			{
				outw(ports.data, *ptr++);
			}
			wait4ns();
		}
		flush();
		waitBusy();
	}

	return true;
}

bool ATADevice::readSector(uint32_t sector, void* data)
{
	return accessSectors(sector, 1, true, data);
}

bool ATADevice::readSectors(uint32_t sector, uint32_t count, void* data)
{
	return accessSectors(sector, count, true, data);
}

bool ATADevice::writeSector(uint32_t sector, void* data)
{
	return accessSectors(sector, 1, false, data);
}

bool ATADevice::writeSectors(uint32_t sector, uint32_t count, void* data)
{
	return accessSectors(sector, count, false, data);
}

uint32_t ATADevice::getFATSector(uint32_t number, uint32_t size, uint32_t sector)
{
	return (number * size) + sector;
}

FATEntryType ATADevice::getFATEntry(uint32_t index)
{
	uint32_t fatSector = mbr.reservedSectorCount + index;
	uint8_t	 fatData[512];
	uint32_t clusterCount = getClusterCount();
	if (readSectors(fatSector, 1, fatData))
	{
		uint32_t record;
		memcpy(&record, fatData, 4);
		if (record == 0x0)
		{
			return Free;
		}
		else if (record >= 0x2 && record <= clusterCount)
		{
			return Allocated;
		}
		else if (record >= clusterCount + 1 && record <= 0xFFFFFF6)
		{
			return Reserved;
		}
		else if (record == 0xFFFFFF7)
		{
			return Defective;
		}
		else if (record >= 0xFFFFFF8 && record <= 0xFFFFFFE)
		{
			return Reserved;
		}
		else
		{
			return EOF;
		}
	}
	return Defective;
}

uint32_t ATADevice::getClusterCount()
{
	uint32_t dataSectors = mbr.sectorCount - (mbr.reservedSectorCount + getFATSize());
	return dataSectors / mbr.sectorsPerCluster;
}

uint32_t ATADevice::getFATSize() { return mbr.tableCount * mbr.bigSectorsPerTable; }
