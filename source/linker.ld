/* The bootloader will look at this image and start execution at the symbol
   designated as the entry point. */
ENTRY(start)

/* Tell where the various sections of the object files will be put in the final
   kernel image. */
SECTIONS
{
	. = 1M; /* Start at 2MB */

	start = .;

	.text BLOCK(4K) : ALIGN(4K)
	{
	text = .;
		stext = .;
		*(.multiboot);
		*(.text);
		etext = .;
	}

	.rodata ALIGN (1K) :
	{
		*(.rodata);
	}

	.data ALIGN (1K) :
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
