#include <cmd.h>
#include <cstdio.h>
#include <shell.h>
#include <sys.h>

#define CHECK_ARGS(e, min, max)                                                                    \
	if (argCount < min || argCount > max)                                                          \
	{                                                                                              \
		warning("Invalid number of arguments for '%s'. Wanted between %d and %d, got %d.", e, min, \
			max, argCount);                                                                        \
		return;                                                                                    \
	}

// List of commands available to the user
static const char* g_commands[] = {
	"exit",
	"help",
	"clear",
	"cat",
	"cd",
	"pwd",
	"ls",
};

// Current working directory
static char g_cwd[MAX_FILENAME_LENGTH];
// Current file descriptor
static int32_t g_cfd;

void CMD::init()
{
	// Default to "/" ([ '/' , '\0' ])
	memset(g_cwd, 0, MAX_FILENAME_LENGTH);
	g_cwd[0] = '/';

	// 3 == root directory
	g_cfd = 3;
}

void CMD::run(const char* cmd)
{
	// Print the entire command to the terminal
	printf(">>> %s\n", cmd);

	// Extract all of the arguments from the command line string
	const auto args = (char**)malloc(MAX_CMD_ARGS); // 16 == maximum number of arguments
	memset(args, 0, MAX_CMD_ARGS);
	int32_t argCount = 0;
	if (!parseCmdArgs(cmd, args, &argCount))
	{
		free(args);
		return; // No command entered
	}

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
		cd(args[1]);
	}
	// List current directory
	else if (strcmp(exe, "ls"))
	{
		// CHECK_ARGS("ls", 1, 2);
		ls(g_cfd);
	}

	// Free all arguments
	for (int32_t i = 0; i < MAX_CMD_ARGS; i++)
	{
		free(args[i]); // Free each argument string
	}
	free(args); // Free the array of arguments
}

bool CMD::parseCmdArgs(const char* cmd, char* args[], int32_t* argCount)
{
	if (!cmd || !args || !argCount)
	{
		return false;
	}

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
			const size_t length = end - start;
			args[argIndex] = static_cast<char*>(malloc(length + 1));
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

	return *argCount > 0; // Return true if we found any arguments
}

bool CMD::isValidExecutable(const char* exe)
{
	for (auto& command : g_commands)
	{
		if (strcmp(exe, command))
		{
			return true;
		}
	}
	return false;
}

bool CMD::isRootDir(const char* path) { return strcmp("/", path); }

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

void CMD::cat(const char* path) { warning("cat: Not implemented yet."); }

void CMD::pwd() { printf("pwd: %s (%d)\n", g_cwd, g_cfd); }

void CMD::cd(const char* path)
{
	if (!path)
	{
		warning("cd: No path specified");
		return;
	}

	CDCommand command;
	if (strcmp("..", path))
	{
		command = CD_UP;
	}
	else if (strcmp(".", path))
	{
		command = CD_SAME;
	}
	else
	{
		command = CD_DOWN;
	}

	/**
	 * TODO: This currently only searches for directories in
	 * the current working directory. It should be updated
	 * to search for directories in the specified path (relative
	 * or absolute).
	 */
	Array<File*> files = readdir(g_cfd);
	if (files.empty())
	{
		warning("cd: No files found in directory '%s'", path);
		return;
	}

	for (const File* file : files)
	{
		// Skip non-directory files.
		if (!file->isDirectory)
		{
			continue;
		}

		// Exact name comparison.
		if (!strcmp(path, file->name))
		{
			continue;
		}

		switch (command)
		{
			// Change to the parent directory.
			case CD_UP:
			{
				// Check if we are already at the root directory. If we are, just return.
				if (isRootDir(g_cwd))
				{
					warning("cd: Cannot go up from root directory");
					return;
				}

				// Get the last '/' in the current working directory and replace it with a null
				// terminator, effectively removing the last directory from the path.
				char* index = strrchr(g_cwd, '/');
				if (index - g_cwd > 0)
				{
					*index = '\0';
				}
				// If we are at the root directory, set the current working directory to "/".
				else
				{
					g_cwd[0] = '/';
					g_cwd[1] = '\0';
				}

				// Set the current file descriptor to the parent directory.
				g_cfd = file->fd;
				printf("cd: %s\n", g_cwd);
				return;
			}
			// Change to the specified directory.
			case CD_DOWN:
			{
				// If we are not at the root directory, add a '/' to the current working directory.
				if (!isRootDir(g_cwd))
				{
					strcat(g_cwd, "/");
				}
				// Add the directory name to the current working directory.
				// At this point, g_cwd should be `/old/new`.
				strcat(g_cwd, file->name);

				//  Set the current file descriptor to the new directory.
				g_cfd = file->fd;
				printf("cd: %s\n", g_cwd);
				return;
			}
			// If the path is the same as the current working directory, just return.
			case CD_SAME:
			{
				return;
			}
		}
	}

	// If we get here, it means we didn't find any matching directories within the current
	// working directory. Issue a warning.
	warning("cd: No dir found matching '%s'", path);
}

void CMD::ls(const int32_t fd)
{
	Array<File*> files = readdir(fd);
	for (const auto& file : files)
	{
		const auto color = file->isDirectory ? VGA_COLOR_CYAN : VGA_COLOR_GREEN;
		Shell::setForeColor(color);
		printf(" %s", file->name);
		Shell::resetColor();
	}
	printf("\n");
}
