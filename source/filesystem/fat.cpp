#include <assert.h>
#include <bitmask.h>
#include <cstring.h>
#include <fat.h>
#include <filesystem.h>
#include <idt.h>
#include <math.h>
#include <memory.h>
#include <stdio.h>

#define GET_DEVICE(index) ATADevice* d = IDE::getDevice(index)

static FAT32::ShortEntry* g_rootEntry;

void FAT32::init()
{
	GET_DEVICE(0);

	ShortEntry data[FAT_ENTRIES_PER_SECTOR];
	if (!d->readSectors(d->rootDirectorySector, 1, &data))
	{
		panic("Failed to read root directory sector.");
	}
	memcpy(g_rootEntry, &data[0], sizeof(ShortEntry));
}

bool FAT32::openFile(const String& filename, void* file)
{
	GET_DEVICE(0);

	// Split the filename by `/`. This will result in an
	// array of each path component we need to search for.
	Array<String> components = filename.split('/');

	if (components.empty())
	{
		warning("No components in path %s", filename.data());
		return false;
	}

	// TODO: Move this to global space as it's redundant
	// Parse the root directory
	ShortEntry data[FAT_ENTRIES_PER_SECTOR];
	if (!d->readSectors(d->rootDirectorySector, 1, &data))
	{
		panic("Failed to read root directory sector.");
	}
	ShortEntry* rootDirectory = &data[0];
	ShortEntry* currentEntry = rootDirectory;

	FATFile	 fatFile;
	uint32_t sector = 0;

	// Iterate through the path components attempting to find
	// the corresponding entry. If a component is a directory,
	// step into that directory by setting the current cluster
	// to that entry's cluster.
	for (const auto& c : components)
	{
		if (!findFile(currentEntry, c, &fatFile))
		{
			return false;
		}

		if (Bitmask::test((uint8_t)fatFile.entry.attribute, (uint8_t)Attribute::Directory))
		{
			*currentEntry = fatFile.entry;
			continue;
		}
		break;
	}

	// If the file entry indicates the file is empty, just return.
	if (!fatFile.entry.fileSize)
	{
		warning("File is empty.");
		return false;
	}

	// Extract relevant file data and properties.
	File* f = (File*)file;
	f->size = fatFile.entry.fileSize;

	// Get the corresponding sector for the entry's cluster.
	sector = FAT32::getClusterSector(fatFile.entry.cluster());
	uint32_t sectorCount = CEILDIV(f->size, d->bootSector.bytesPerSector);

	// Allocate memory for the file.
	f->data = new char[fatFile.entry.fileSize];

	// Finally read the sector corresponding to this entry,
	// storing the result in f->data.
	return d->readSectors(sector, sectorCount, f->data);
}

// Converts the given `longName` to a FAT 8.3 compatible
// short name.
// Examples:
// [test.txt] (4)          => [TEST    TXT] (11)
// [longFileName.txt] (12) => [LONGFI~1TXT] (11)
String FAT32::toShortName(const String& longName)
{
	// Split the longname into its name and extension
	auto components = FileSystem::splitExt(longName);
	// Force null-terminate the NAME at 8 chars
	components.a[8] = '\0';
	// Force null-terminate the EXT at 3 chars
	components.b[3] = '\0';

	// Remove invalid characters and whitespace
	String base = sanitize(components.a, 8);
	String ext = sanitize(components.b, 3);

	// TODO: Account for unique tilde incrementation
	bool   useTilde = false;
	String shortBase, shortExt;

	// We use a tilde (~) if the base component resized.
	if (base.size() > 8)
	{
		useTilde = true;
	}
	// Again, force resize to 8/3
	base.resize(8, ' ');
	ext.resize(3, ' ');

	if (useTilde)
	{
		// If we're using a tilde, at most we can use 6 characters
		// because (123456~N) == 8 characters.
		size_t allowed = 6;

		// If the base is smaller than 6 characters, set the
		// allowed count to the size of the base.
		if (base.size() < allowed)
		{
			allowed = base.size();
		}

		// Extract only [0..Allowed] count of chars from
		// the base and append "~1" to it.
		base = base.substr(0, allowed) + "~1";
	}

	// Return the concatenated base and extension.
	return base + ext;
}

String FAT32::toLongName(char* shortName)
{
	String base;
	String ext;

	// Copy the first (up-to) 8 characters of the short name.
	for (size_t i = 0; i < 8; i++)
	{
		if (shortName[i] == ' ')
		{
			break;
		}
		base.append(tolower(shortName[i]));
	}

	// If there's an extension present, append a '.' and
	// copy the extension.
	if (shortName[8] != ' ' && shortName[8] != '\0')
	{
		base.append('.');
		for (size_t i = 8; i < 11; i++)
		{
			if (shortName[i] == ' ')
			{
				break;
			}
			ext.append(tolower(shortName[i]));
		}
	}

	// Return the concatenated base and extension components.
	return base + ext;
}

// Sanitizes the name component (either the base or the ext)
// to remove invalid characters and force all characters
// to uppercase.
char* FAT32::sanitize(const String& component, size_t count)
{
	char*  result = (char*)std::kmalloc(count);
	size_t j = 0;
	for (size_t i = 0; i < count; i++)
	{
		char c = component[i];

		// We've hit the end, append a null-terminator
		// and break.
		// If this character is invalid, continue.
		if (!isValidChar(c))
		{
			continue;
		}
		// Append an uppercase version of `c`.
		c = toupper(c);
		result[j] = c;
		j++;
	}
	result[count] = '\0';
	return result;
}

// Checks if the given char `c` is a valid FAT short name
// character.
bool FAT32::isValidChar(char c)
{
	// If the char is alpha-numeric (i.e. a-z || A-Z || 0-9)
	if (isalnum(c))
	{
		return true;
	}

	// If the character is not one of the below allowed
	// filename characters.
	return !(c == '.' || c == '"' || c == '/' || c == '\\' || c == '[' || c == ']' || c == ':'
		|| c == ';' || c == '=' || c == ',');
}

bool FAT32::findFile(ShortEntry* current, const String& name, FATFile* file)
{
	GET_DEVICE(0);

	String shortName = toShortName(name);

	Array<FATFile> files;
	if (!readDirectory(*current, files))
	{
		return false;
	}

	for (const auto& f : files)
	{
		if (f.name == name)
		{
			*file = f;
			return true;
		}
	}

	return false;
}

bool FAT32::isLongEntry(void* buffer)
{
	// The entry is a long entry if either:
	// - The first character of the entry is 0.
	// - The 'attribute' component of a FAT entry is at offset 11 (0xB). This
	// character is equal to 15 (0xF).
	return *(uint8_t*)buffer == Attribute::LastEntry
		|| *((uint8_t*)buffer + 11) == Attribute::LongFileName;
}

void FAT32::parseLongEntry(LongEntry* entry, uint32_t count, char* filename)
{
	// While we still have entries to parse...
	size_t charCount = 0;
	while (count)
	{
		// Decrement count
		count--;

		// Each entry's data is 26 bytes long with 'wchar_t',
		// but it's 13 bytes long with 'char'. Compute
		// the offset within the `filename` array for this loop.
		uint32_t offset = count * 13;

		// 26 bytes for wchar_t data.
		char wdata[26];

		// Copy each data segment into the wchar_t buffer.
		// Data0 == 0..9
		// Data1 == 10..21
		// Data2 == 22..25
		memcpy(wdata, entry->data0, 10);
		memcpy(wdata + 10, entry->data1, 12);
		memcpy(wdata + 22, entry->data2, 4);

		// Copy every other char to the filename.
		for (int i = 0; i < 13; i++)
		{
			*(filename + offset + i) = wdata[i * 2];
			charCount++;
		}

		// Increment the entry pointer to move to the next
		// long filename entry.
		entry++;
	}

	filename[charCount] = 0;
}

// Given the cluster number `n`, get the next cluster.
uint32_t FAT32::getNextCluster(uint32_t n)
{
	GET_DEVICE(0);
	uint32_t offset = n * 4; // ??? why multiply by 4?
	uint32_t sector = d->bootSector.reservedSectorCount + (offset / d->bootSector.bytesPerSector);
	uint32_t offsetInSector = offset % d->bootSector.bytesPerSector;

	// Assumes the sector is 512 bytes.
	uint8_t buffer[512];
	if (!d->readSector(sector, buffer))
	{
		return FAT_END_OF_CLUSTER; // End of chain
	}

	uint32_t* entry = (uint32_t*)(buffer + offsetInSector);
	uint32_t  next = *entry & FAT_END_OF_CLUSTER;
	return next;
}

// Given the cluster number `n`, get the corresponding
// sector number.
uint32_t FAT32::getClusterSector(uint32_t n)
{
	GET_DEVICE(0);
	return ((n - 2) * d->bootSector.sectorsPerCluster) + d->firstDataSector;
}

// Returns the total number of clusters (sectors for data)
// in this partition.
uint32_t FAT32::getClusterCount()
{
	GET_DEVICE(0);
	uint32_t dataSectors =
		d->bootSector.sectorCount - (d->bootSector.reservedSectorCount + getSize());
	return dataSectors / d->bootSector.sectorsPerCluster;
}

// Returns the total number of sectors in this partition.
uint32_t FAT32::getSize()
{
	GET_DEVICE(0);
	return d->bootSector.tableCount * d->bootSector.bigSectorsPerTable;
}

FAT32::ShortEntry* FAT32::getRootEntry() { return g_rootEntry; }

bool FAT32::readDirectory(const ShortEntry& entry, Array<FATFile>& files)
{
	GET_DEVICE(0);

	// Start at `startCluster`. This will be updated as we traverse a cluster
	// and do not (yet) find the matching entry.
	uint32_t cluster = entry.cluster();
	if (!cluster)
	{
		return false;
	}

	// Create a buffer which will hold all of the data for this cluster (all 16 sectors).
	uint8_t buffer[8192];
	while (cluster < FAT_END_OF_CLUSTER)
	{
		// Get the first sector of this cluster.
		uint32_t firstSector = FAT32::getClusterSector(cluster);

		// Read each sector (16 total) of this cluster into `buffer` at the offset
		// [n * BPS] where `n` is the current sector index and BPS is the bytes
		// per sector.
		for (uint8_t sector = 0; sector < d->bootSector.sectorsPerCluster; sector++)
		{
			if (!d->readSector(
					firstSector + sector, buffer + (sector * d->bootSector.bytesPerSector)))
			{
				warning("Unable to read sector %d", firstSector + sector);
				return false;
			}
		}

		// Compute the number of entries we will look through
		// in this cluster.
		uint32_t entriesPerCluster =
			(d->bootSector.sectorsPerCluster * d->bootSector.bytesPerSector) / sizeof(ShortEntry);

		// Cast the raw buffer we read above to a short entry
		// array we can iterate through.
		ShortEntry* shortEntries = (ShortEntry*)buffer;
		for (uint32_t i = 0; i < entriesPerCluster; i++)
		{
			ShortEntry* shortEntry = &shortEntries[i];
			char*		filename = (char*)std::kmalloc(MAX_FILENAME);
			if (isLongEntry((uint8_t*)shortEntry))
			{
				LongEntry* longEntry = (LongEntry*)shortEntry;
				size_t	   count = longEntry->id - 0x40;
				i += count;
				parseLongEntry(longEntry, count, filename);

				FATFile file;
				file.entry = shortEntries[i];
				file.name = String(filename, strlen(filename));

				files.add(file);
				continue;
			}

			FATFile file;
			file.entry = *shortEntry;
			file.name = toLongName((char*)shortEntry->name);
			files.add(file);
		}

		// Move to the next cluster.
		cluster = FAT32::getNextCluster(cluster);
	}

	return true;
}
