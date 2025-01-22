/* The bootloader will look at this image and start execution at the symbol
   designated as the entry point. */
ENTRY(_start)
OUTPUT_FORMAT(elf32-i386)

MULTIBOOT_ADDR = 0x100000;
HIGHER_HALF_OFFSET = 0xC0000000;

/* Tell where the various sections of the object files will be put in the final
   kernel image. */
SECTIONS
{
	/* Ensure multiboot starts at 1MB. */
	. = MULTIBOOT_ADDR;

	.multiboot ALIGN(4K) : {
		KEEP(*(.multiboot))
	}


	kernelStart = . + HIGHER_HALF_OFFSET;

	.text ALIGN(4K) :
	{
		*(.text)
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
