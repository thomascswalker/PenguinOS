#pragma once

#include <array.h>
#include <fat.h>
#include <filesystem.h>
#include <string.h>

namespace CMD
{

	enum CDCommand
	{
		CD_UP,
		CD_DOWN,
		CD_SAME,
	};

	void init();
	void processCmd(const char* cmd);
	void parseCmdArgs(const char* cmd, char* args[], int32_t* argCount);
	bool isValidExecutable(const char* exe);

	char* getCwd(bool relative);

	void exit();
	void help();
	void clear();
	void cat(const char* path);
	void pwd();
	void cd(const char* path);
	void ls(const char* path);
} // namespace CMD