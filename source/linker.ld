/* The bootloader will look at this image and start execution at the symbol
   designated as the entry point. */
ENTRY(_start)

/* Tell where the various sections of the object files will be put in the final
   kernel image. */
SECTIONS
{
	. = 0xC0100000; /* Start at 3GB + 1MB */

	/* Start kernel */
	kernel_start = .;

	/* Align relative to 0xC0000000) */
	 .text ALIGN(4K) :  AT(ADDR(.text) - 0xC0000000)
	{
		*(.multiboot)
		*(.text)
	}

	
	.boot ALIGN(4K) :  AT(ADDR(.boot) - 0xC0000000)
	{
		*(.boot)
	}

		/* Align relative to 0xC0000000) */
	.data ALIGN(4K) : AT(ADDR(.data) - 0xC0000000)
	{
		*(.data)
	}

		/* Align relative to 0xC0000000) */
	.rodata ALIGN(4K) : AT(ADDR(.rodata) - 0xC0000000)
	{
		*(.rodata)
	}

		/* Align relative to 0xC0000000) */
	.bss ALIGN(4K) : AT(ADDR(.bss) - 0xC0000000)
	{
		*(COMMON)
		*(.bss)
	}

	kernel_end = .;
}
