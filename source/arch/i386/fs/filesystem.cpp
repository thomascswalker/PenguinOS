#include <filesystem.h>
#include <ide.h>
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
	auto d = IDE::getDevice(0);
	return d->readFile(path.string(), file);
}

bool FileSystem::isFile(const Path& path) { return path.string().split('.').size(); }

Pair<String, String> FileSystem::splitExt(const Path& path)
{
	String p = path.string();
	size_t dot = p.findLast('.');
	String basename, ext;
	if (dot == String::npos || dot == 0)
	{
		basename = p;
	}
	else
	{
		basename = p.substring(0, dot);
		ext = p.substring(dot + 1);
	}
	return Pair(basename, ext);
}

Path Path::parent() { return Path(); }

String Path::filename() { return String(); }

String Path::stem() { return String(); }

String Path::extension() { return String(); }
