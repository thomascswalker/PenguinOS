#include <pit.h>
#include <shell.h>
#include <testProcess.h>

void testProcess()
{
	while (true)
	{
		printf("Tick: %d\n", PIT::getTimerTick());
	}
}