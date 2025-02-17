#pragma once

#include <stddef.h>
#include <stdint.h>
#include <string.h>

struct File
{
	char* name;
	char  ext[3];
	void* data;
};

namespace FileSystem
{
	void  init();
	File* readFile(const String& filename);
	File* findFile(String& filename);
	bool  isFile(String& name);

} // namespace FileSystem