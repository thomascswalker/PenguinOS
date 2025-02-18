#include <fat.h>
#include <filesystem.h>
#include <math.h>
#include <memory.h>
#include <stdio.h>

ATADevice* device;

void FileSystem::init()
{
	IDE::init();
	device = IDE::getDevice(0);
}

bool FileSystem::readFile(const Path& path, File* file)
{
	return FAT32::readFile(path.string(), file);
}

bool FileSystem::isFile(const Path& path) { return path.string().split('.').size(); }

bool FileSystem::isDirectory(const Path& path) { return !path.string().split('.').size(); }

Pair<String, String> FileSystem::splitExt(const Path& path)
{
	String p = path.string();
	size_t pos = p.findLast('.');

	if (pos == String::npos || pos == p.size() - 1)
	{
		return { p, "" };
	}
	return { p.substr(0, pos), p.substr(pos + 1) };
}

Path Path::parent() { return Path(); }

String Path::filename()
{
	size_t dot = path.findLast('.');
	return path.substr(0, dot);
}

String Path::stem()
{
	size_t dot = path.findLast('.');
	return path.substr(dot + 1);
}

String Path::extension() { return String(); }
