#include <filesystem.h>
#include <ide.h>
#include <memory.h>
#include <stdio.h>

uint8_t* fileTable;

void FileSystem::readRoot()
{
	ATADevice* drive = IDE::getDevice(0);
	drive->select();
	if (!drive)
	{
		error("Device 0 is invalid.");
		return;
	}

	char string[512] = "This222 is a string on disc.";
	char data[512];
	drive->writeSectors(1, 1, string);
	if (drive->readSectors(1, 1, data))
	{
		debugs(data);
	}
}