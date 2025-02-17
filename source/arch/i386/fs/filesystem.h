#pragma once

#include <pair.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

struct File
{
	char* name;
	char  ext[3];
	void* data;
};

class Path
{
	String path;

public:
	Path   parent();
	String filename();
	String stem();
	String extension();

	Path() = default;
	Path(const String& inPath) : path(inPath) {}

	String string() const { return path; }
	char*  cstr() const { return path.cstr(); }
	operator String() { return path; }
};

namespace FileSystem
{
	void  init();
	File* readFile(const Path& path);
	File* findFile(const Path& path);

	bool isFile(const Path& path);
	bool isDirectory(const Path& path);

	Pair<String, String> splitExt(const Path& path);

} // namespace FileSystem