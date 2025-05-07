#pragma once

#include <stdint.h>

#define MAX_CMD_ARGS 16
#define MAX_CMD_LENGTH 128
#define MAX_FILE_DEPTH 32

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
	bool parseCmdArgs(const char* cmd, char* args[], int32_t* argCount);
	bool isValidExecutable(const char* exe);

	char* getCwd(bool relative);
	bool  isRootDir(const char* path);

	void exit();
	void help();
	void clear();
	void cat(const char* path);
	void pwd();
	void cd(const char* path);
	void ls(int32_t fd);
} // namespace CMD