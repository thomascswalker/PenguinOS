#if defined(__linux__)
#error "You are not using a cross-compiler."
#endif

#if !defined(__i386__)
// #error "This needs to be compiled with ix86-elf."
#endif

#include <terminal.h>

void kernel_main(void)
{
    init_terminal();
    printfc("Welcome to PengOS.\n", VGA_COLOR_LIGHT_GREEN);
    printf("\tThomas Walker\n");
    printf("t\tTest");
}
