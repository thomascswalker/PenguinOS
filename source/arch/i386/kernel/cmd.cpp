#include <cmd.h>
#include <shell.h>
#include <sys.h>

using namespace FAT32;

#define CHECK_ARGS(e, min, max)                                                                    \
	if (argCount < min || argCount > max)                                                          \
	{                                                                                              \
		warning("Invalid number of arguments for '%s'. Wanted between %d and %d, got %d.", e, min, \
			max, argCount);                                                                        \
		return;                                                                                    \
	}

static const char* g_commands[] = {
	"exit",
	"help",
	"clear",
	"cat",
	"cd",
	"pwd",
	"ls",
};
static const size_t g_commandsCount = sizeof(g_commands) / sizeof(const char*);

// Current working directory
static CMD::CWD g_cwd;

void CMD::init()
{
	// g_cwd = CWD(); // Reinstantiate

	// // Default to "//0"
	// memset(g_cwd.path, 0, 128);
	// g_cwd.path[0] = '/';

	// // Start at root entry
	// g_cwd.entry = *getRootEntry();
}

void CMD::processCmd(const char* cmd)
{
	// Print the entire command to the terminal
	printf(">>> %s\n", cmd);

	// Extract all of the arguments from the command line string

	char** args = new char*[16]; // 16 == maximum number of arguments
	memset(args, 0, 16);
	int32_t argCount = 0;

	parseCmdArgs(cmd, args, &argCount);
	debug("CMD: %s: %d args", cmd, argCount);
	if (argCount == 0)
	{
		free(args);
		delete[] args;
		return; // No command entered
	}

	for (size_t i = 0; i < argCount; i++)
	{
		printf("args[%d]=%s, ", i, args[i]);
	}
	printf("\n");

	// Check if the command is valid
	const char* exe = args[0];
	if (!isValidExecutable(exe))
	{
		Shell::setForeColor(VGA_COLOR_LIGHT_RED);
		printf("Unknown command '%s'.\n", exe);
		Shell::resetColor();
	}

	// Exit OS
	if (strcmp(exe, "exit"))
	{
		// CHECK_ARGS("exit", 0, 0);
		exit();
	}
	// Dipslay help
	else if (strcmp(exe, "help"))
	{
		// CHECK_ARGS("help", 0, 0);
		help();
	}
	// Clear terminal
	else if (strcmp(exe, "clear"))
	{
		// CHECK_ARGS("clear", 0, 0);
		clear();
	}
	// conCATenate
	else if (strcmp(exe, "cat"))
	{
		// CHECK_ARGS("cat", 1, 1);
		cat(args[0]);
	}
	// Print working directory
	else if (strcmp(exe, "pwd"))
	{
		// CHECK_ARGS("pwd", 0, 0);
		pwd();
	}
	// Change directory
	else if (strcmp(exe, "cd"))
	{
		// CHECK_ARGS("cd", 1, 1);
		cd(args[0]);
	}
	// List current directory
	else if (strcmp(exe, "ls"))
	{
		CHECK_ARGS("ls", 1, 2);
		if (argCount == 2)
		{
			ls(args[1]);
		}
		else
		{
			ls("/");
		}
	}
	for (int32_t i = 0; i < argCount; i++)
	{
		delete[] args[i]; // Free each argument
		args[i] = nullptr;
	}
	free(args);
	args = nullptr;
}

Array<String> CMD::parseCmdArgs(const String& cmd) { return cmd.split(' '); }

void CMD::parseCmdArgs(const char* cmd, char* args[], int32_t* argCount)
{
	size_t		argIndex = 0;
	const char* start = cmd;
	const char* end = cmd;

	while (*end != '\0')
	{
		// Skip leading spaces
		while (*start == ' ' && *start != '\0')
		{
			start++;
		}

		// Find the end of the current argument
		end = start;
		while (*end != ' ' && *end != '\0')
		{
			end++;
		}

		// If we found a valid argument, add it to the array
		if (start != end)
		{
			size_t length = end - start;
			args[argIndex] = new char[length + 1]; // Allocate memory for the argument
			strcpy(args[argIndex], start);
			args[argIndex][length] = '\0'; // Null-terminate the string
			argIndex++;
		}

		// Move to the next argument
		start = end;
	}

	// Null-terminate the array of arguments
	args[argIndex] = nullptr;
	*argCount = argIndex;
}

bool CMD::isValidExecutable(const char* exe)
{
	debug("Checking if '%s' is a valid executable", exe);
	for (size_t i = 0; i < g_commandsCount; i++)
	{
		if (strcmp(exe, g_commands[i]))
		{
			return true;
		}
	}
	return false;
}

const char* CMD::getCWD() { return g_cwd.path; }

void CMD::exit() { sysexit(); }

void CMD::help()
{
	printf("HELP: exit  - Exit the system.\n"
		   "      help  - Display list of commands.\n"
		   "      clear - Clear the display of text.\n"
		   "      cat   - Print the contents of the specified file.\n"
		   "      pwd   - Print the current directory.\n"
		   "      cd    - Change to the specified directory.\n"
		   "      ls    - Print the files within the current directory.\n");
}

void CMD::clear() { Shell::clearDisplay(); }

void CMD::cat(const String& path)
{
	// File file;
	// if (!FileSystem::openFile(path, &file))
	// {
	// 	warning("cat: %s: No such file or directory", path.data());
	// 	return;
	// }
	// printf("%s\n", file.data);
}

void CMD::pwd() { printf("pwd: %s\n", g_cwd.path); }

void CMD::cd(const String& path)
{
	// ShortEntry entry;

	// if (!findEntry(g_cwd.entry.cluster(), path, &entry))
	// {
	// 	warning("cd: %s: No such directory", path.data());
	// 	return;
	// }

	// if (!entry.isValid())
	// {
	// 	warning("cd: Entry found is invalid: %s, %x", entry.name, entry.attribute);
	// 	return;
	// }

	// if (!Bitmask::test((uint8_t)entry.attribute, (uint8_t)Attribute::Directory))
	// {
	// 	warning("cd: %s: Not a directory", path.data());
	// 	return;
	// }

	// memcpy(&g_cwd.entry, &entry, sizeof(ShortEntry));
}

void CMD::ls(const char* path)
{
	Array<File*> files = readdir(path);
	for (auto file : files)
	{
		if (file->isDirectory && file->name[0] == '.')
		{
			continue; // Skip hidden directories
		}
		auto color = file->isDirectory ? VGA_COLOR_CYAN : VGA_COLOR_GREEN;
		Shell::setForeColor(color);
		printf(" %s", file->name);
		Shell::resetColor();
	}
	printf("\n");
	files.clear();
}
