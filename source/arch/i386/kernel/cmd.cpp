#include <cmd.h>
#include <filesystem.h>
#include <shell.h>
#include <sys.h>

static const Array<String> cmds = {
	"exit",
	"help",
	"clear",
	"cat",
	"cd",
	"cwd",
	"ls",
};

int CMD::processCmd(const Array<String>& args)
{
	if (args.size() == 0)
	{
		warning("Invalid argument count.");
		return 1;
	}
	String exe = args[0];
	bool   present = false;
	for (uint32_t i = 0; i < cmds.size(); i++)
	{
		String cmd(cmds[i]);
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
		return 1;
	}

	if (exe == "exit")
	{
		exit();
		return 0;
	}
	if (exe == "help")
	{
		printf("HELP: exit - Exit the system.\n");
		return 0;
	}
	if (exe == "clear")
	{
		Shell::clearDisplay();
		return 0;
	}
	if (exe == "cat")
	{
		if (args.size() != 2)
		{
			warning("Invalid number of arguments for 'cat'. Wanted 2, got %d.", args.size());
			return 1;
		}
		String filename = args[1];
		File   f;
		if (!FileSystem::openFile(filename, &f))
		{
			warning("cat: %s: No such file or directory", filename.cstr());
			return 1;
		}
		printf("%s\n", f.data);
	}

	return 0;
}