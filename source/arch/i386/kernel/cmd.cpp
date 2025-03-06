#include <cmd.h>
#include <shell.h>
#include <sys.h>

#define CHECK_ARGS(e, n)                                                                        \
	if (args.size() != n)                                                                       \
	{                                                                                           \
		warning("Invalid number of arguments for '%s'. Wanted %d, got %d.", e, n, args.size()); \
		return;                                                                                 \
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
	g_cwd = CWD(); // Reinstantiate

	// Default to "//0"
	g_cwd.path = "/";

	// Start at root entry
	g_cwd.entry = *FAT32::getRootEntry();
}

void CMD::processCmd(const String& cmd)
{
	// Extract all of the arguments from the command line string
	Array<String> args = parseCmdArgs(cmd);
	if (args.size() == 0)
	{
		return;
	}

	// Check if the command is valid
	String exe = *args.at(0);
	for (int i = 0; i < exe.size(); i++)
	{
		printf("%d|", exe[i]);
	}

	debug("%d => %s", args.size(), exe.data());
	if (!isValidExecutable(exe))
	{
		Shell::setForeColor(VGA_COLOR_LIGHT_RED);
		printf("Unknown command '%s'.\n", exe.data());
		Shell::resetColor();
		return;
	}

	// Exit OS
	if (strcmp(exe.data(), "exit"))
	{
		CHECK_ARGS("exit", 1);
		exit();
	}
	// Dipslay help
	else if (strcmp(exe.data(), "help"))
	{
		CHECK_ARGS("help", 1);
		help();
	}
	// Clear terminal
	else if (strcmp(exe.data(), "clear"))
	{
		CHECK_ARGS("clear", 1);
		clear();
	}
	// conCATenate
	else if (strcmp(exe.data(), "cat"))
	{
		CHECK_ARGS("cat", 2);
		cat(args[1]);
	}
	// Print working directory
	else if (strcmp(exe.data(), "pwd"))
	{
		CHECK_ARGS("pwd", 1);
		pwd();
	}
	// Change directory
	else if (strcmp(exe.data(), "cd"))
	{
		CHECK_ARGS("cd", 2);
		cd(args[1]);
	}
	// List current directory
	else if (strcmp(exe.data(), "ls"))
	{
		CHECK_ARGS("ls", 1);
		ls();
	}
}

Array<String> CMD::parseCmdArgs(const String& cmd) { return cmd.split(' '); }

bool CMD::isValidExecutable(const String& exe)
{
	for (size_t i = 0; i < g_commandsCount; i++)
	{
		if (strcmp(exe.data(), g_commands[i]))
		{
			return true;
		}
	}
	return false;
}

String CMD::getCWD() { return g_cwd.path; }

void CMD::exit() { sysexit(); }

void CMD::help()
{
	printf("HELP: exit - Exit the system.\n"
		   "      help - Display list of commands.\n"
		   "      clear - Clear the display of text.\n"
		   "      cat - Print the contents of the specified file.\n"
		   "      pwd - Print the current directory.\n"
		   "      cd - Change to the specified directory.\n"
		   "      ls - Print the files within the current directory.\n");
}

void CMD::clear() { Shell::clearDisplay(); }

void CMD::cat(const String& path)
{
	File file;
	if (!FileSystem::openFile(path, &file))
	{
		warning("cat: %s: No such file or directory", path.data());
		return;
	}
	printf("%s\n", file.data);
}

void CMD::pwd() { printf("pwd: %s\n", g_cwd.path.data()); }

void CMD::cd(const String& path)
{
	FAT32::FATFile file;

	if (!findFile(&g_cwd.entry, path, &file))
	{
		warning("cd: %s: No such directory", path.data());
		return;
	}

	if (!file.entry.isValid())
	{
		warning("cd: Entry found is invalid: %s, %x", file.name.data(), file.entry.attribute);
		return;
	}

	if (!Bitmask::test((uint8_t)file.entry.attribute, (uint8_t)FAT32::Attribute::Directory))
	{
		warning("cd: %s: Not a directory", path.data());
		return;
	}

	memcpy(&g_cwd.entry, &file.entry, sizeof(FAT32::ShortEntry));
}

void CMD::ls()
{
	Array<FAT32::FATFile> files;
	if (!readDirectory(g_cwd.entry, files))
	{
		warning("ls: Invalid current directory: %x, Attr:%x, Clus:%d", g_cwd.entry,
			g_cwd.entry.attribute, g_cwd.entry.cluster());
		return;
	}

	for (const auto& file : files)
	{
		if (!file.entry.isValid())
		{
			continue;
		}
		switch (file.entry.attribute)
		{
			case FAT32::Attribute::Directory: // Folders
				{
					Shell::setForeColor(VGA_COLOR_CYAN);
					printf(" - %s\n", file.name.data());
					Shell::resetColor();
					break;
				}
			default: // Files and other types
				{
					Shell::setForeColor(VGA_COLOR_GREEN);
					printf(" - %s\n", file.name.data());
					Shell::resetColor();
					break;
				}
		}
	}
}
