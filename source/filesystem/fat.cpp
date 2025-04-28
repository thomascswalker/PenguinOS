#include <assert.h>
#include <bitmask.h>
#include <cstdio.h>
#include <cstring.h>
#include <fat.h>
#include <filesystem.h>
#include <idt.h>
#include <math.h>
#include <memory.h>

using namespace FAT32;

FAT32FileSystem::FAT32FileSystem()
{
	m_device = IDE::getDevice(0);

	ShortEntry data[FAT_ENTRIES_PER_SECTOR];
	if (!m_device->readSectors(m_device->rootDirectorySector, 1, &data))
	{
		panic("Failed to read root directory sector.");
	}

	memcpy(&m_rootEntry, &data[0], sizeof(ShortEntry));
}

bool FAT32FileSystem::getEntryFromPath(const char* filename, FAT32::ShortEntry* entry)
{
	if (strcmp(filename, "/"))
	{
		memcpy(entry, &m_rootEntry, sizeof(ShortEntry));
		return true;
	}

	// Split the filename by `/`. This will result in an
	// array of each path component we need to search for.
	String str(filename);
	if (str.endsWith('/'))
	{
		str = str.substr(0, str.size() - 1);
	}
	Array<String> components = str.split('/');

	if (components.empty())
	{
		warning("FAT32: No components in path %s", str.data());
		return 0;
	}

	// TODO: Move this to global space as it's redundant
	// Parse the root directory
	ShortEntry data[FAT_ENTRIES_PER_SECTOR];
	if (!m_device->readSectors(m_device->rootDirectorySector, 1, &data))
	{
		panic("FAT32: Failed to read root directory sector.");
	}
	ShortEntry* rootDirectory = &data[0];
	uint32_t	currentCluster = rootDirectory->cluster();

	uint32_t sector = 0;

	// Iterate through the path components attempting to find
	// the corresponding entry. If a component is a directory,
	// step into that directory by setting the current cluster
	// to that entry's cluster.
	for (const auto& c : components)
	{
		if (!findEntry(currentCluster, c, entry))
		{
			warning("FAT32: Unable to find entry %s", c.data());
			return false;
		}

		if (Bitmask::test((uint8_t)entry->attribute, FA_Directory))
		{
			currentCluster = entry->cluster();
			continue;
		}
		break;
	}

	return true;
}

// Given the cluster number `n`, get the next cluster.
uint32_t FAT32FileSystem::getNextCluster(uint32_t n)
{
	uint32_t offset = n * 4; // ??? why multiply by 4?
	uint32_t sector =
		m_device->bootSector.reservedSectorCount + (offset / m_device->bootSector.bytesPerSector);
	uint32_t offsetInSector = offset % m_device->bootSector.bytesPerSector;

	// Assumes the sector is 512 bytes.
	uint8_t buffer[512];
	if (!m_device->readSector(sector, buffer))
	{
		return FAT_END_OF_CLUSTER; // End of chain
	}

	uint32_t* entry = (uint32_t*)(buffer + offsetInSector);
	uint32_t  next = *entry & FAT_END_OF_CLUSTER;
	return next;
}

// Given the cluster number `n`, get the corresponding
// sector number.
uint32_t FAT32FileSystem::getClusterSector(uint32_t n)
{
	return ((n - 2) * m_device->bootSector.sectorsPerCluster) + m_device->firstDataSector;
}

// Returns the total number of clusters (sectors for data)
// in this partition.
uint32_t FAT32FileSystem::getClusterCount()
{
	uint32_t dataSectors =
		m_device->bootSector.sectorCount - (m_device->bootSector.reservedSectorCount + getSize());
	return dataSectors / m_device->bootSector.sectorsPerCluster;
}

// Returns the total number of sectors in this partition.
uint32_t FAT32FileSystem::getSize()
{
	return m_device->bootSector.tableCount * m_device->bootSector.bigSectorsPerTable;
}

// FAT32 implementation of syscall `open`.
int32_t FAT32FileSystem::open(const char* filename)
{
	ShortEntry entry;
	if (!getEntryFromPath(filename, &entry))
	{
		return -1;
	}
	int32_t cluster = entry.cluster();
	for (auto& pair : m_openEntries)
	{
		if (pair.a == cluster)
		{
			return cluster;
		}
	}
	m_openEntries.add({ cluster, entry });

	return cluster;
}

// FAT32 implementation of syscall `read`.
size_t FAT32FileSystem::read(int32_t fd, void* buffer, size_t size)
{
	uint32_t sector = getClusterSector(fd);
	if (m_device->readSector(sector, buffer))
	{
		return size;
	}
	return 0;
}

// FAT32 implementation of syscall `close`.
void FAT32FileSystem::close(int32_t fd)
{
	for (size_t i = 0; i < m_openEntries.size(); i++)
	{
		if (m_openEntries[i].a == fd)
		{
			m_openEntries.remove(i);
			return;
		}
	}
}

/*
Retrieves a list of files and directories within a specified
cluster in a FAT32 file system. It reads the cluster's sectors,
processes both short and long directory entries to extract
file metadata, and returns an `Array<File*>` containing the
parsed file objects.

@param cluster The cluster number to read files from.

@returns An array containing the files and directories
		  found in the specified cluster.
*/
Array<File*> FAT32FileSystem::getFilesInDirectory(int32_t cluster)
{
	Array<File*>	 files;		  // Array of files to return
	Array<LongEntry> longEntries; // Array of long entries to store long filenames

	// Create a buffer which will hold all of the data for
	// this cluster (all 16 sectors).
	char buffer[512];

	// Get the first sector of this cluster.
	uint32_t firstSector = getClusterSector(cluster);

	// Read each sector (16 total) of this cluster into `buffer` at the offset
	// [n * BPS] where `n` is the current sector index and BPS is the bytes
	// per sector.
	for (uint8_t sector = 0; sector < m_device->bootSector.sectorsPerCluster; sector++)
	{
		if (!m_device->readSector(
				firstSector + sector, buffer + (sector * m_device->bootSector.bytesPerSector)))
		{
			warning("Unable to read sector %d", firstSector + sector);
			return files;
		}
	}

	// Compute the number of entries we will look through
	// in this cluster.
	uint32_t entriesPerCluster =
		(m_device->bootSector.sectorsPerCluster * m_device->bootSector.bytesPerSector)
		/ sizeof(ShortEntry);

	// Cast the raw buffer we read above to a short entry
	// array we can iterate through.
	ShortEntry* current = (ShortEntry*)buffer;
	while (current != nullptr)
	{
		// No more entries
		if (!current->isValid())
		{
			break;
		}

		// Store long entries until we reach another short entry.
		// These long entries comprise the long name of the filename
		// for the next short entry that's found.
		if (isLongEntry((uint8_t*)current))
		{
			LongEntry* longEntry = (LongEntry*)current;
			longEntries.add(*longEntry);
			current++;
			continue;
		}

		File* f = new File();
		// If there's one or more long entries, we need to parse
		// the long name and set it to the filename of this file.
		if (longEntries.size() > 0)
		{
			// We have a long entry, parse it
			int32_t count = longEntries.size();
			f->name = parseLongEntryName(&longEntries[0], count);
			longEntries.clear();
		}
		// Otherwise, we have a short entry. Parse it and set it to the filename.
		else
		{
			f->name = parseShortEntryName(current);
		}

		// Set the file's attributes and size.
		int32_t cluster = current->cluster();
		f->size = current->fileSize;
		f->fd = cluster;
		f->isDirectory = Bitmask::test(current->attribute, FA_Directory);

		files.add(f);

		// Go to the next entry
		current++;
	}

	return files;
}

// Similar to getFilesInDirectory, but takes a filename
// rather than a cluster number.
Array<File*> FAT32FileSystem::getFilesInDirectoryFromName(const char* filename)
{
	ShortEntry entry;
	if (strcmp(filename, "/"))
	{
		entry = m_rootEntry;
	}
	else if (!getEntryFromPath(filename, &entry))
	{
		warning("Unable to get entry from path %s", filename);
		return Array<File*>();
	}

	return getFilesInDirectory(entry.cluster());
}

// Returns the size of the file associated with the given
// file descriptor `fd`.
size_t FAT32FileSystem::getFileSize(int32_t fd)
{
	for (const auto& pair : m_openEntries)
	{
		if (pair.a == fd)
		{
			return pair.b.fileSize;
		}
	}
	return 0;
}

size_t FAT32FileSystem::getFileSizeFromName(const char* filename)
{
	ShortEntry entry;
	if (!getEntryFromPath(filename, &entry))
	{
		return 0;
	}
	return entry.fileSize;
}

// Converts the given `longName` to a FAT 8.3 compatible
// short name.
// Examples:
// [test.txt] (4)          => [TEST    TXT] (11)
// [longFileName.txt] (12) => [LONGFI~1TXT] (11)
char* FAT32FileSystem::toShortName(const char* longName)
{
	char* result = new char[12];
	memset(result, ' ', 11);
	result[11] = '\0';

	auto size = strlen(longName);
	auto dot = strchri(longName, '.');

	// Extension
	// '.profile'
	if (dot == 0)
	{
		size -= 1;
		dot = 9;
	}
	// 'file.txt'
	else if (dot > 0)
	{
		int ext = size - dot - 1;
		ext = std::min(ext, 3);
		for (int i = 0; i < ext; i++)
		{
			result[8 + i] = toupper(longName[dot + 1 + i]);
		}
	}

	if (dot > -1)
	{
		size = dot;
	}

	if (size > 8)
	{
		for (int i = 0; i < 6; i++)
		{
			result[i] = toupper(longName[i]);
		}
		result[6] = '~';
		result[7] = '1'; // Assume only one version
	}
	else
	{
		for (int i = 0; i < size; i++)
		{
			result[i] = toupper(longName[i]);
		}
	}

	return result;
}

// Sanitizes the name component (either the base or the ext)
// to remove invalid characters and force all characters
// to uppercase.
char* FAT32FileSystem::sanitize(const String& component, size_t count)
{
	char*  result = (char*)malloc(count);
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
bool FAT32FileSystem::isValidChar(char c)
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

bool FAT32FileSystem::findEntry(uint32_t startCluster, const String& name, ShortEntry* entry)
{
	// Convert the input `name` to the FAT 8.3 short name specification.
	char* shortName = toShortName(name);

	// Start at `startCluster`. This will be updated as we traverse a cluster
	// and do not (yet) find the matching entry.
	uint32_t cluster = startCluster;

	// Create a buffer which will hold all of the data for this cluster (all 16 sectors).
	uint8_t buffer[8192];
	while (cluster < FAT_END_OF_CLUSTER)
	{
		// Get the first sector of this cluster.
		uint32_t firstSector = getClusterSector(cluster);

		// Read each sector (16 total) of this cluster into `buffer` at the offset
		// [n * BPS] where `n` is the current sector index and BPS is the bytes
		// per sector.
		for (uint8_t sector = 0; sector < m_device->bootSector.sectorsPerCluster; sector++)
		{
			if (!m_device->readSector(
					firstSector + sector, buffer + (sector * m_device->bootSector.bytesPerSector)))
			{
				warning("Unable to read sector %d", firstSector + sector);
				return false;
			}
		}

		// Compute the number of entries we will look through
		// in this cluster.
		uint32_t entriesPerCluster =
			(m_device->bootSector.sectorsPerCluster * m_device->bootSector.bytesPerSector)
			/ sizeof(ShortEntry);

		// Cast the raw buffer we read above to a short entry
		// array we can iterate through.
		ShortEntry* entries = (ShortEntry*)buffer;
		for (uint32_t i = 0; i < entriesPerCluster; i++)
		{
			ShortEntry* current = &entries[i];

			// No more entries
			if (!current->isValid())
			{
				continue;
			}
			// Ignore long filename entries
			else if (isLongEntry((uint8_t*)current))
			{
				continue;
			}

			// Only compare the first 8 characters of each name

			if (strncmp(shortName, (char*)current->name, 8))
			{
				// If we have a match, we've found the corresponding
				// entry. Copy `current` memory into our in/out `entry`.
				memcpy(entry, current, 32);
				return true;
			}
		}

		// Move to the next cluster.
		cluster = getNextCluster(cluster);
	}

	return false;
}

bool FAT32FileSystem::isLongEntry(uint8_t* buffer)
{
	// The entry is a long entry if either:
	// - The first character of the entry is 0.
	// - The 'attribute' component of a FAT entry is at offset 11 (0xB). This
	// character is equal to 15 (0xF).
	return ((ShortEntry*)buffer)->attribute == FA_LongFileName;
}

char* FAT32FileSystem::parseLongEntryName(LongEntry* entry, uint32_t count)
{
	// While we still have entries to parse...
	char* filename = new char[count * 13 + 1];
	filename[count * 13] = '\0';

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
		}

		// Increment the entry pointer to move to the next
		// long filename entry.
		entry++;
	}
	// Append a null-terminator to the end of the filename.
	return filename;
}

char* FAT32FileSystem::parseShortEntryName(FAT32::ShortEntry* entry)
{
	char* filename = new char[12];
	memset(filename, 0, 12);

	if (strncmp(".       ", (const char*)entry->name, 8))
	{
		filename[0] = '.';
		return filename;
	}
	else if (strncmp("..	  ", (const char*)entry->name, 8))
	{
		filename[0] = '.';
		filename[1] = '.';
		return filename;
	}

	int32_t mainEnd = 0;
	int32_t extEnd = 0;
	for (mainEnd = 8; mainEnd > 0 && entry->name[mainEnd - 1] == ' '; mainEnd--)
		;
	memcpy(filename, entry->name, mainEnd);
	for (extEnd = 3; extEnd > 0 && entry->name[extEnd - 1 + 8] == ' '; extEnd--)
		;
	if (extEnd != 0)
	{
		filename[mainEnd] = '.';
	}
	memcpy(filename + mainEnd + 1, (void*)(entry->name + 8), extEnd);
	for (int32_t i = 0; i < strlen(filename); i++)
	{
		filename[i] = tolower(filename[i]);
	}
	return filename;
}

bool FAT32FileSystem::readDirectory(const ShortEntry& entry, Array<ShortEntry>& entries)
{
	// Start at `startCluster`. This will be updated as we traverse a cluster
	// and do not (yet) find the matching entry.
	uint32_t cluster = entry.cluster();
	if (!cluster)
	{
		return false;
	}
	uint8_t buffer[8192];

	while (cluster < FAT_END_OF_CLUSTER)
	{
		// Get the first sector of this cluster.
		uint32_t firstSector = getClusterSector(cluster);

		if (!firstSector)
		{
			return false;
		}

		// Read each sector (16 total) of this cluster into `buffer` at the offset
		// [n * BPS] where `n` is the current sector index and BPS is the bytes
		// per sector.
		for (uint8_t sector = 0; sector < m_device->bootSector.sectorsPerCluster; sector++)
		{
			if (!m_device->readSector(
					firstSector + sector, buffer + (sector * m_device->bootSector.bytesPerSector)))
			{
				warning("Unable to read sector %d", firstSector + sector);
				return false;
			}
		}

		// Compute the number of entries we will look through
		// in this cluster.
		uint32_t entriesPerCluster =
			(m_device->bootSector.sectorsPerCluster * m_device->bootSector.bytesPerSector)
			/ sizeof(ShortEntry);

		// Cast the raw buffer we read above to a short entry
		// array we can iterate through.
		ShortEntry* entryBuffer = (ShortEntry*)buffer;
		for (uint32_t i = 0; i < entriesPerCluster; i++)
		{
			ShortEntry* current = &entryBuffer[i];

			// No more entries
			if (!current->isValid())
			{
				continue;
			}
			// Ignore long filename entries
			else if (isLongEntry((uint8_t*)current))
			{
				continue;
			}

			entries.add(*current);
		}

		// Move to the next cluster.
		cluster = getNextCluster(cluster);
	}

	return true;
}
