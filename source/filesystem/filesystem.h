#pragma once

#include <pair.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

namespace FileSystem
{
	Pair<String, String> splitExt(const String& path);
	String				 join(const Array<String>& components);
} // namespace FileSystem