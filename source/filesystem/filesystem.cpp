#include <fat.h>
#include <filesystem.h>
#include <math.h>
#include <memory.h>
#include <stdio.h>

Pair<String, String> FileSystem::splitExt(const String& path)
{
	size_t pos = path.findLast('.');
	if (pos == String::npos || pos == path.size() - 1)
	{
		return { path, "" };
	}
	return { path.substr(0, pos), path.substr(pos + 1) };
}

String FileSystem::join(const Array<String>& components)
{
	String path("/");
	for (size_t i = 0; i < components.size(); i++)
	{
		String comp = components[i];
		if (comp.startsWith('/'))
		{
			comp.erase(0, 1);
		}
		path.append(comp);
		if (i < components.size() - 1)
		{
			path.append('/');
		}
	}
	path[path.size()] = 0;
	return path;
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

void FileSystem::init()
{
	IDE::init();
	FAT32::init();
}

bool FileSystem::openFile(const String& path, File* file) { return FAT32::openFile(path, file); }

bool FileSystem::isFile(const String& path) { return path.split('.').size(); }

bool FileSystem::isDirectory(const String& path) { return !path.split('.').size(); }
