/* The bootloader will look at this image and start execution at the symbol
   designated as the entry point. */
ENTRY(_start)
OUTPUT_FORMAT(elf32-i386)

KERNEL_VIRTUAL_BASE = 0xC0000000;

/* Tell where the various sections of the object files will be put in the final
   kernel image. */
SECTIONS
{
	/* Ensure multiboot starts at 1MB. */
	. = 0x100000;

	.multiboot ALIGN(4K) : {
		KEEP(*(.multiboot))
	}

   /* The kernel will live at 3GB + 1MB in the virtual
      address space, which will be mapped to 1MB in the
      physical address space. */
	. += KERNEL_VIRTUAL_BASE;
	kernelStart = .;

	/* Align relative to 0xC0000000) */
	.text ALIGN(4K) : AT(ADDR(.text) - KERNEL_VIRTUAL_BASE)
	{
		*(.text)
	}

	/* Align relative to 0xC0000000) */
	.data ALIGN(4K) : AT(ADDR(.data) - KERNEL_VIRTUAL_BASE)
	{
		*(.data)
		*(.rodata)
	}

	/* Align relative to 0xC0000000) */
	.bss ALIGN(4K) : AT(ADDR(.bss) - KERNEL_VIRTUAL_BASE)
	{
		*(COMMON)
		*(.bss)
	}

	kernelEnd = .;

}
