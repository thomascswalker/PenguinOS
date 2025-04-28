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
	void		  processCmd(const char* cmd);
	Array<String> parseCmdArgs(const String& cmd);
	void		  parseCmdArgs(const char* cmd, char* args[], int32_t* argCount);
	bool		  isValidExecutable(const char* exe);
	const char*	  getCWD();

	void exit();
	void help();
	void clear();
	void cat(const String& path);
	void pwd();
	void cd(const String& path);
	void ls(const char* path);
} // namespace CMD