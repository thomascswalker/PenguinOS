#include <assert.h>
#include <bitmask.h>
#include <cstring.h>
#include <fat.h>
#include <filesystem.h>
#include <idt.h>
#include <math.h>
#include <memory.h>
#include <stdio.h>

bool FAT32::isLongEntry(uint8_t* buffer)
{
	return *buffer == FA_LastEntry || *(buffer + 11) == FA_LongFileName;
}

void FAT32::parseLongEntry(FATLongEntry* entry, uint32_t count, char* filename)
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

bool FAT32::findEntry(uint32_t startCluster, const String& name, FATShortEntry* entry)
{
	auto	 d = IDE::getDevice(0);
	String	 fname = FAT32::toShortName(name);
	uint32_t cluster = startCluster;

	uint8_t buffer[4096];
	while (cluster < 0x0FFFFFF8)
	{
		uint32_t firstSector = FAT32::getClusterSector(cluster);
		for (uint8_t sector = 0; sector < d->mbr.sectorsPerCluster; sector++)
		{
			if (!d->readSector(firstSector + sector, buffer + (sector * d->mbr.bytesPerSector)))
			{
				warning("Unable to read sector %d", firstSector + sector);
				return false;
			}
		}

		uint32_t entriesPerCluster =
			(d->mbr.sectorsPerCluster * d->mbr.bytesPerSector) / sizeof(FATShortEntry);
		FATShortEntry* entries = (FATShortEntry*)buffer;

		for (uint32_t i = 0; i < entriesPerCluster; i++)
		{
			FATShortEntry* current = &entries[i];

			if (current->name[0] == FA_Empty) // No more entries
			{
				return false;
			}
			if (current->name[0] == FA_Deleted) // Deleted entry
			{
				continue;
			}
			if (current->name[0] == '.'
				&& (current->name[1] == ' ' || current->name[1] == '.')) // . & .. entries
			{
				continue;
			}
			if (isLongEntry((uint8_t*)current)) // Long file name entry
			{
				continue;
			}

			if (strcmp(fname.cstr(), (char*)current->name, 8))
			{
				memcpy(entry, current, 32);
				return true;
			}
		}

		cluster = FAT32::getNextCluster(cluster);
	}

	return false;
}

bool FAT32::readFile(const String& filename, void* file)
{
	auto		  d = IDE::getDevice(0);
	Array<String> components = filename.split('/');

	if (components.empty())
	{
		warning("No components in path %s", filename.cstr());
		return false;
	}

	// Parse the root directory
	FATShortEntry data[FAT_ENTRIES_PER_SECTOR];
	if (!d->readSectors(d->rootDirectorySector, 1, &data))
	{
		panic("Failed to read root directory sector.");
	}
	FATShortEntry* rootDirectory = &data[0];
	uint32_t	   currentCluster = rootDirectory->firstCluster();

	FATShortEntry entry;
	uint32_t	  sector = 0;
	uint32_t	  count = 0;

	for (const auto& c : components)
	{
		if (!findEntry(currentCluster, c, &entry))
		{
			warning("No entry for %s.", c.cstr());
			return false;
		}

		if (std::Bitmask::test(entry.attr, FA_Directory))
		{
			currentCluster = entry.firstClusterLow;
			continue;
		}
		break;
	}
	if (!entry.fileSize)
	{
		warning("File is empty.");
		return false;
	}

	File* f = (File*)file;
	f->size = entry.fileSize;
	sector = FAT32::getClusterSector(entry.firstClusterLow);
	count = CEILDIV(entry.fileSize, d->mbr.bytesPerSector);
	f->data = new char[entry.fileSize];
	return d->readSector(sector, f->data);
}

bool FAT32::isValidChar(char c)
{
	if (isalnum(c))
	{
		return true;
	}
	return !(c == '.' || c == '"' || c == '/' || c == '\\' || c == '[' || c == ']' || c == ':'
		|| c == ';' || c == '=' || c == ',');
}

String FAT32::toShortName(const String& longName)
{
	auto components = FileSystem::splitExt(longName);
	components.b.terminate();
	String base = sanitize(components.a);
	String ext = sanitize(components.b);

	// TODO: Account for unique tilde incrementation
	bool   useTilde = false;
	String shortBase, shortExt;

	if (base.size() > 8 || components.a.size() != base.size())
	{
		useTilde = true;
	}
	base.resize(8, ' ');
	ext.resize(3, ' ');

	if (useTilde)
	{
		size_t allowed = 6;
		if (base.size() < allowed)
		{
			allowed = base.size();
		}
		base = base.substr(0, allowed) + "~1";

		if (base.size() > 8)
		{
			base = base.substr(0, 8);
		}

		base = ext.substr(0, 3);
	}

	return base + ext;
}

String FAT32::sanitize(const String& component)
{
	String result;

	for (size_t i = 0; i < component.size(); i++)
	{
		char c = component[i];
		if (c == '\0')
		{
			result.append('\0');
			break;
		}
		if (!isValidChar(c))
		{
			continue;
		}
		result.append(toupper(c));
	}
	return result;
}

uint32_t FAT32::getNextCluster(uint32_t cluster)
{
	ATADevice* d = IDE::getDevice(0);

	uint32_t offset = cluster * 4;
	uint32_t sector = d->mbr.reservedSectorCount + (offset / d->mbr.bytesPerSector);
	uint32_t offsetInSector = offset % d->mbr.bytesPerSector;

	uint8_t buffer[512];
	if (!d->readSector(sector, buffer))
	{
		return 0x0FFFFFFF; // End of chain
	}

	uint32_t* entry = (uint32_t*)(buffer + offsetInSector);
	uint32_t  next = *entry & 0x0FFFFFFF;
	return next;
}

uint32_t FAT32::getSector(uint32_t number, uint32_t size, uint32_t sector)
{
	return (number * size) + sector;
}

FATEntryType FAT32::getEntryType(uint32_t index)
{
	ATADevice* d = IDE::getDevice(0);
	uint32_t   sector = d->mbr.reservedSectorCount + index;
	uint8_t	   data[512];
	uint32_t   clusterCount = FAT32::getClusterCount();

	if (d->readSectors(sector, 1, data))
	{
		uint32_t record;
		memcpy(&record, data, 4);
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

uint32_t FAT32::getClusterSector(uint32_t n)
{
	ATADevice* d = IDE::getDevice(0);
	return ((n - 2) * d->mbr.sectorsPerCluster) + d->firstDataSector;
}

uint32_t FAT32::getClusterCount()
{
	ATADevice* d = IDE::getDevice(0);
	uint32_t   dataSectors = d->mbr.sectorCount - (d->mbr.reservedSectorCount + getSize());
	return dataSectors / d->mbr.sectorsPerCluster;
}

uint32_t FAT32::getSize()
{
	ATADevice* d = IDE::getDevice(0);
	return d->mbr.tableCount * d->mbr.bigSectorsPerTable;
}
