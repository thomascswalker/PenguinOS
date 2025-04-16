#pragma once

#include <pair.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

struct File
{
	char*	 name;
	char	 ext[3];
	uint32_t size;
	void*	 data;
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
	Path(const char* inPath) : path(inPath) {}
	Path(const String& inPath) : path(inPath) {}

	void append(const String& p)
	{
		if ((path[path.size() - 1]) != '/')
		{
			path.append('/');
		}
		path.append(p);
	}

	String string() const { return path; }
	char*  cstr() const { return path.data(); }
	operator String() { return path; }
};

namespace FileSystem
{
	bool openFile(const String& path, File* file);
	bool isFile(const String& path);
	bool isDirectory(const String& path);

	Pair<String, String> splitExt(const String& path);
	String				 join(const Array<String>& components);

} // namespace FileSystem