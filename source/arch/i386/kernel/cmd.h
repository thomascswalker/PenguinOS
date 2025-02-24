#pragma once

#include <array.h>
#include <fat.h>
#include <string.h>

namespace CMD
{
	struct CWD
	{
		FAT32::ShortEntry* entry;
		String			   path;
	};

	void   init();
	void   processCmd(const Array<String>& args);
	String getCWD();
} // namespace CMD