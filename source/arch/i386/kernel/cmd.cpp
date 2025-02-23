#include <cmd.h>
#include <filesystem.h>
#include <shell.h>
#include <sys.h>

static const Array<String> g_commands = {
	"exit",
	"help",
	"clear",
	"cat",
	"cd",
	"cwd",
	"ls",
};

void CMD::processCmd(const Array<String>& args)
{
	if (args.size() == 0)
	{
		warning("Invalid argument count.");
		return;
	}
	String exe = args[0];
	bool   present = false;
	for (const auto& cmd : g_commands)
	{
		if (exe == cmd)
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
		String filename = args[1];
		File   f;
		if (!FileSystem::openFile(filename, &f))
		{
			warning("cat: %s: No such file or directory", filename.cstr());
			return;
		}
		printf("%s\n", f.data);
	}
}