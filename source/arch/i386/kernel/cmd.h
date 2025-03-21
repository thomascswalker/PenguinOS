#pragma once

#include <array.h>
#include <fat.h>
#include <filesystem.h>
#include <string.h>

namespace CMD
{
	struct CWD
	{
		FAT32::ShortEntry entry;
		char			  path[128];
	};

	void		  init();
	void		  processCmd(const String& cmd);
	Array<String> parseCmdArgs(const String& cmd);
	bool		  isValidExecutable(const String& exe);
	const char*	  getCWD();

	void exit();
	void help();
	void clear();
	void cat(const String& path);
	void pwd();
	void cd(const String& path);
	void ls();
} // namespace CMD