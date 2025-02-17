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

File* FileSystem::readFile(const String& filename) { return nullptr; }

File* FileSystem::findFile(String& filename) { return nullptr; }

bool FileSystem::isFile(String& name) { return name.split('.').size(); }
