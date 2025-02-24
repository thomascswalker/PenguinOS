#include <cmd.h>
#include <filesystem.h>
#include <shell.h>
#include <sys.h>

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
		exit();
		return;
	}
	if (exe == "help")
	{
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
		if (args.size() != 2)
		{
			warning("Invalid number of arguments for 'cat'. Wanted 2, got %d.", args.size());
			return;
		}
		File		file;
		const char* filename = args[1].cstr();
		if (!FileSystem::openFile(Path(filename), &file))
		{
			warning("cat: %s: No such file or directory", filename);
			return;
		}
		printf("%s\n", file.data);
	}
}