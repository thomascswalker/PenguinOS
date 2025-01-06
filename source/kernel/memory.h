#include <multiboot.h>
#include <stdio.h>

void init_memory(multiboot_info_t* boot_info)
{
	for (uint32_t i = 0; i < boot_info->mmap_length; i += sizeof(multiboot_mmap_entry_t))
	{
		multiboot_mmap_entry_t* mmmt = (multiboot_mmap_entry_t*)(boot_info->mmap_addr + i);
		debug("(%d) Low addr: 0x%x", i, mmmt->addr_low);
		debug("(%d) High addr: 0x%x", i, mmmt->addr_high);
		debug("(%d) Low length: 0x%x", i, mmmt->len_low);
		debug("(%d) High length: 0x%x", i, mmmt->len_high);
		debug("(%d) Size: %d", i, mmmt->size);
		debug("(%d) Type: %d", i, mmmt->type);
	}
}