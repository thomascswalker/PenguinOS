#include <cmd.h>
#include <fat.h>
#include <filesystem.h>
#include <shell.h>
#include <sys.h>

using namespace FAT32;

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
	"cwd",
	"ls",
};
static const size_t g_commandsCount = sizeof(g_commands) / sizeof(const char*);

// Current working directory
static CMD::CWD g_cwd;

void CMD::init()
{
	g_cwd.path = "/";
	g_cwd.entry = getRootEntry();
}

void CMD::processCmd(const Array<String>& args)
{
	if (args.size() == 0)
	{
		warning("Invalid argument count.");
		return;
	}
	String exe = args[0];
	bool   present = false;
	for (size_t i = 0; i < g_commandsCount; i++)
	{
		if (exe == g_commands[i])
		{
			present = true;
			break;
		}
	}
	if (!present)
	{
		Shell::setForeColor(VGA_COLOR_LIGHT_RED);
		printf("Unknown command '%s'.\n", exe.cstr());
		Shell::resetColor();
		return;
	}

	if (exe == "exit")
	{
		CHECK_ARGS("exit", 1);
		exit();
		return;
	}
	if (exe == "help")
	{
		CHECK_ARGS("help", 1);
		printf("HELP: exit - Exit the system.\n");
		return;
	}
	if (exe == "clear")
	{
		Shell::clearDisplay();
		return;
	}
	if (exe == "cat")
	{
		CHECK_ARGS("cat", 2);
		File		file;
		const char* filename = args[1].cstr();
		if (!FileSystem::openFile(Path(filename), &file))
		{
			warning("cat: %s: No such file or directory", filename);
			return;
		}
		printf("%s\n", file.data);
	}
	if (exe == "cwd")
	{
		CHECK_ARGS("cwd", 1);
		printf("cwd: %s\n", g_cwd.path.cstr());
		return;
	}
	if (exe == "cd")
	{
		CHECK_ARGS("cd", 2);
		error("'cd' not implemented yet.");
		return;
	}
	if (exe == "ls")
	{
		CHECK_ARGS("cwd", 1);
		Array<ShortEntry> entries;
		if (readDirectory(g_cwd.entry, entries))
		{
			for (const auto& entry : entries)
			{
				char name[9];
				memcpy(name, (void*)entry.name, 8);
				name[8] = 0;
				printf("%s | %x\n", name, entry.attribute);
			}
		}
	}
}

String CMD::getCWD() { return g_cwd.path; }
