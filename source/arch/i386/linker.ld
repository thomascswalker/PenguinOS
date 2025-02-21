/* The bootloader will look at this image and start execution at the symbol
   designated as the entry point. */
ENTRY(_start)
OUTPUT_FORMAT(elf32-i386)

HIGHER_HALF_OFFSET = 0xC0000000;

/* Tell where the various sections of the object files will be put in the final
   kernel image. */
SECTIONS
{
	/* Ensure multiboot starts at 1MB. */
	. = 0x100000;

	.multiboot ALIGN(4K) :
	{
		*(.multiboot)
	}

	kernelStart = . + HIGHER_HALF_OFFSET;

	.text ALIGN(4K) :
	{
		*(.text)
	}

	.init_array :
	{
		__init_array_start = .;
		KEEP(*(.init_array))
		__init_array_end = .;
	}

	.data ALIGN(4K) :
	{
		*(.data)
		*(.rodata)
	}

	.bss ALIGN(4K) :
	{
		*(COMMON)
		*(.bss)
	}

	kernelEnd = . + HIGHER_HALF_OFFSET;

}
