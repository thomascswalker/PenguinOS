#include "terminal.h"

void kernel_main(void)
{
	init_terminal();
	printfc("Welcome to PengOS.\n", VGA_COLOR_LIGHT_GREEN);
	printf("\tThomas Walker\n");
	printf("t\tTest");
}
