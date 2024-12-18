#include "stdlib/terminal.h"

void kernel_main(void)
{
	init_terminal();
	println("Welcome to PengOS!");
	println("Made by Thomas Walker.");
}
