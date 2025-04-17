// https://www.gnu.org/software/grub/manual/multiboot/multiboot.html#Example-OS-code
/* multiboot.h - Multiboot header file. */
/* Copyright (C) 1999,2003,2007,2008,2009,2010  Free Software Foundation, Inc.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to
 *  deal in the Software without restriction, including without limitation the
 *  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 *  sell copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL ANY
 *  DEVELOPER OR DISTRIBUTOR BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
 *  IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#pragma once

#include <cstdio.h>
#include <stdint.h>

/* How many bytes from the start of the file we search for the header. */
#define MULTIBOOT_SEARCH 8192
#define MULTIBOOT_HEADER_ALIGN 4

/* The magic field should contain this. */
#define MULTIBOOT_HEADER_MAGIC 0x1BADB002

/* This should be in %eax. */
#define MULTIBOOT_BOOTLOADER_MAGIC 0x2BADB002

/* Alignment of multiboot modules. */
#define MULTIBOOT_MOD_ALIGN 0x00001000

/* Alignment of the multiboot info structure. */
#define MULTIBOOT_INFO_ALIGN 0x00000004

/* Flags set in the ’flags’ member of the multiboot header. */

/* Align all boot modules on i386 page (4KB) boundaries. */
#define MULTIBOOT_PAGE_ALIGN 0x00000001

/* Must pass memory information to OS. */
#define MULTIBOOT_MEMORY_INFO 0x00000002

/* Must pass video information to OS. */
#define MULTIBOOT_VIDEO_MODE 0x00000004

/* This flag indicates the use of the address fields in the header. */
#define MULTIBOOT_AOUT_KLUDGE 0x00010000

/* Flags to be set in the ’flags’ member of the multiboot info structure. */

/* is there basic lower/upper memory information? */
#define MULTIBOOT_INFO_MEMORY 0x00000001
/* is there a boot device set? */
#define MULTIBOOT_INFO_BOOTDEV 0x00000002
/* is the command-line defined? */
#define MULTIBOOT_INFO_CMDLINE 0x00000004
/* are there modules to do something with? */
#define MULTIBOOT_INFO_MODS 0x00000008

/* These next two are mutually exclusive */

/* is there a symbol table loaded? */
#define MULTIBOOT_INFO_AOUT_SYMS 0x00000010
/* is there an ELF section header table? */
#define MULTIBOOT_INFO_ELF_SHDR 0X00000020

/* is there a full memory map? */
#define MULTIBOOT_INFO_MEM_MAP 0x00000040

/* Is there drive info? */
#define MULTIBOOT_INFO_DRIVE_INFO 0x00000080

/* Is there a config table? */
#define MULTIBOOT_INFO_CONFIG_TABLE 0x00000100

/* Is there a boot loader name? */
#define MULTIBOOT_INFO_BOOT_LOADER_NAME 0x00000200

/* Is there a APM table? */
#define MULTIBOOT_INFO_APM_TABLE 0x00000400

/* Is there video information? */
#define MULTIBOOT_INFO_VBE_INFO 0x00000800
#define MULTIBOOT_INFO_FRAMEBUFFER_INFO 0x00001000

struct MultibootHeader
{
	/* Must be MULTIBOOT_MAGIC - see above. */
	uint32_t magic;

	/* Feature flags. */
	uint32_t flags;

	/* The above fields plus this one must equal 0 mod 2^32. */
	uint32_t checksum;

	/* These are only valid if MULTIBOOT_AOUT_KLUDGE is set. */
	uint32_t headerAddress;
	uint32_t loadAddress;
	uint32_t loadEndAddress;
	uint32_t bssEndAddress;
	uint32_t entryAddress;

	/* These are only valid if MULTIBOOT_VIDEO_MODE is set. */
	uint32_t modeType;
	uint32_t width;
	uint32_t height;
	uint32_t depth;
};

/* The symbol table for a.out. */
struct multiboot_aout_symbol_table
{
	uint32_t tabsize;
	uint32_t strsize;
	uint32_t addr;
	uint32_t reserved;
};
typedef struct multiboot_aout_symbol_table multiboot_aout_symbol_table_t;

/* The section header table for ELF. */
struct multiboot_elf_section_header_table
{
	uint32_t num;
	uint32_t size;
	uint32_t addr;
	uint32_t shndx;
};
typedef struct multiboot_elf_section_header_table multiboot_elf_section_header_table_t;

struct MultibootInfo
{
	/* Multiboot info version number */
	uint32_t flags;

	/* Available memory from BIOS */
	uint32_t memLower;
	uint32_t memUpper;

	/* "root" partition */
	uint32_t bootDevice;

	/* Kernel command line */
	uint32_t cmdline;

	/* Boot-Module list */
	uint32_t moduleCount;
	uint32_t moduleAddress;

	union
	{
		multiboot_aout_symbol_table_t		 aout_sym;
		multiboot_elf_section_header_table_t elf_sec;
	} u;

	/* Memory Mapping buffer */
	uint32_t mmapLength;
	uint32_t mmapAddress;

	/* Drive Info buffer */
	uint32_t drivesLength;
	uint32_t drivesAddress;

	/* ROM configuration table */
	uint32_t configTable;

	/* Boot Loader Name */
	uint32_t bootLoaderName;

	/* APM table */
	uint32_t apmTable;

	/* Video */
	uint32_t vbe_control_info;
	uint32_t vbe_mode_info;
	uint16_t vbe_mode;
	uint16_t vbe_interface_seg;
	uint16_t vbe_interface_off;
	uint16_t vbe_interface_len;

	uint32_t framebuffer_addr_low;
	uint32_t framebuffer_addr_high;
	uint32_t framebuffer_pitch;
	uint32_t framebuffer_width;
	uint32_t framebuffer_height;
	uint8_t	 framebuffer_bpp;
#define MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED 0
#define MULTIBOOT_FRAMEBUFFER_TYPE_RGB 1
#define MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT 2
	uint8_t framebuffer_type;
	union
	{
		struct
		{
			uint32_t framebuffer_palette_addr;
			uint16_t framebuffer_palette_num_colors;
		};
		struct
		{
			uint8_t framebuffer_red_field_position;
			uint8_t framebuffer_red_mask_size;
			uint8_t framebuffer_green_field_position;
			uint8_t framebuffer_green_mask_size;
			uint8_t framebuffer_blue_field_position;
			uint8_t framebuffer_blue_mask_size;
		};
	};
};

struct multiboot_color
{
	uint8_t red;
	uint8_t green;
	uint8_t blue;
};

struct MBMEntry // Memory map entry
{
	uint32_t size;
	uint32_t addressLow;
	uint32_t addressHigh;
	uint32_t lengthLow;
	uint32_t lengthHigh;
#define MULTIBOOT_MEMORY_AVAILABLE 1
#define MULTIBOOT_MEMORY_RESERVED 2
#define MULTIBOOT_MEMORY_ACPI_RECLAIMABLE 3
#define MULTIBOOT_MEMORY_NVS 4
#define MULTIBOOT_MEMORY_BADRAM 5
	uint32_t type;
} __attribute__((packed));

struct MBMMap // Memory map
{
	uint32_t count;
	MBMEntry entries[144];
};

struct multiboot_mod_list
{
	/* the memory used goes from bytes ’mod_start’ to ’mod_end-1’ inclusive */
	uint32_t mod_start;
	uint32_t mod_end;

	/* Module command line */
	uint32_t cmdline;

	/* padding to take it to 16 bytes (must be zero) */
	uint32_t pad;
};
typedef struct multiboot_mod_list multiboot_module_t;

/* APM BIOS info. */
struct multiboot_apm_info
{
	uint16_t version;
	uint16_t cseg;
	uint32_t offset;
	uint16_t cseg_16;
	uint16_t dseg;
	uint16_t flags;
	uint16_t cseg_len;
	uint16_t cseg_16_len;
	uint16_t dseg_len;
};

namespace Multiboot
{
	static uint8_t drive;
	static uint8_t part1;
	static uint8_t part2;
	static uint8_t part3;

	static void init(MultibootInfo* info, uint32_t* address, uint32_t* size)
	{

		if ((info->flags & MULTIBOOT_INFO_MODS) == MULTIBOOT_INFO_MODS)
		{
			uint32_t mod1 = *(uint32_t*)(info->moduleAddress + 4);
			*address = (mod1 + 0xFFF) & ~0xFFF;
			*size = (info->memUpper * 1024) - *address;
			return;
		}

		if ((info->flags & MULTIBOOT_INFO_BOOTDEV) == MULTIBOOT_INFO_BOOTDEV)
		{
			printf("Boot loader: %s\n", reinterpret_cast<char*>(info->bootLoaderName));

			drive = (info->bootDevice & 0xFF000000) >> 24;
			part1 = (info->bootDevice & 0x00FF0000) >> 16;
			part2 = (info->bootDevice & 0x0000FF00) >> 8;
			part3 = (info->bootDevice & 0x000000FF);

			switch (drive)
			{
				case 0xE0:
					{
						println("Loaded from CD/DVD.");
						break;
					}
				case 0x80:
					{
						println("Loaded from Partition 1.");
						break;
					}
				case 0x81:
					{
						println("Loaded from Partition 2.");
						break;
					}
				case 0x82:
					{
						println("Loaded from Partition 3.");
						break;
					}
				default:
					{
						break;
					}
			}
		}

		uint32_t end = info->mmapAddress + info->mmapLength;

		MBMEntry* mmap = (MBMEntry*)info->mmapAddress;
		while ((uintptr_t)mmap < end)
		{
			if (mmap->lengthLow > *size && mmap->type == MULTIBOOT_MEMORY_AVAILABLE)
			{
				*size = mmap->lengthLow;
				*address = mmap->addressLow;
			}
			mmap = (MBMEntry*)((uint32_t)mmap + mmap->size + sizeof(uint32_t));
		}
	}
} // namespace Multiboot