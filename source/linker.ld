/* The bootloader will look at this image and start execution at the symbol
   designated as the entry point. */
ENTRY(start)

/* Tell where the various sections of the object files will be put in the final
   kernel image. */
SECTIONS
{
	. = 1M; /* Start at 1MB */

	start = .;

	.text BLOCK(4K) : ALIGN(4K)
	{
		stext = .;
		*(.multiboot);
		*(.text);
		etext = .;
	}

	.rodata BLOCK(4K) : ALIGN(4K)
	{
		*(.rodata);
	}

	.data BLOCK(4K) : ALIGN(4K)
	{
		sdata = .;
		*(.data); 
		edata = .;
	}

	.bss :
	{
		sbss = .;
		*(COMMON);
		*(.bss);
		ebss = .;
	}

	end = .;
}
