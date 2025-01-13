/* The bootloader will look at this image and start execution at the symbol
   designated as the entry point. */
ENTRY(_start)

/* Tell where the various sections of the object files will be put in the final
   kernel image. */
SECTIONS
{
	. = 1M; /* Start at 1MB */

	.multiboot ALIGN(4K) :
	{
		*(.multiboot)
	}

	/* Start kernel */
	. += 0xC0000000;

	kernelStart = .;

	/* Align relative to 0xC0000000) */
	.text ALIGN(4K) : AT(ADDR(.text) - 0xC0000000)
	{
		*(.text)
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

	kernelEnd = .;
}
