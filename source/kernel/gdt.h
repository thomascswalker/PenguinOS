// https://www.thejat.in/learn/implement-gdt-in-multiboot-bootloader

#include <system.h>

// struct gdt_entry
// {
// 	uint16 limit_low;
// 	uint16 base_low;
// 	uint8  base_middle;
// 	uint8  access;
// 	uint8  granularity;
// 	uint8  base_high;
// } __attribute__((packed));
// typedef struct gdt_entry gdt_entry_t;

// struct gdt_ptr
// {
// 	uint16 limit;
// 	uint8  base;
// } __attribute__((packed));
// typedef struct gdt_ptr gdt_ptr_t;

// gdt_entry_t gdt[3]; // Define the three entries for our GDT
// gdt_ptr_t	gp;		// Define the pointer to the GDT

// // Defined in gdt.asm
// // Reloads the segment registers.
// extern void gdt_flush();

// void gdt_set_gate(int32 num, uint32 base, uint32 limit, uint8 access, uint8 gran)
// {
// 	gdt[num].base_low = (base & 0xFFFF);		 // Set the lower 16 bits of the base.
// 	gdt[num].base_middle = (base >> 16) & 0xFF;	 // Set the next 8 bits of the base.
// 	gdt[num].base_high = (base >> 24) & 0xFF;	 // Set the high 8 bits of the base.
// 	gdt[num].limit_low = (limit & 0xFFFF);		 // Set the lower 16 bits of the limit.
// 	gdt[num].granularity = (limit >> 16) & 0x0F; // Set the high 4 bits of the limit in granularity.
// 	gdt[num].granularity |= (gran & 0xF0);		 // Combine the granularity flags.
// 	gdt[num].access = access;					 // Set the access flags.
// }

// void gdt_install()
// {
// 	/* GDT pointer and limits */
// 	gp.limit = (64 * 3) - 1; // Calculate the total size of the GDT.

// 	// Set the base address of the GDT
// 	// Without cast: some compilers may issue a warning or error.
// 	// gp.base = &gdt;
// 	// With explicit cast: ensures the address is treated as an integer.
// 	// gp.base = (uint8)&gdt; // Set the base address of the GDT.

// 	/* NULL segment: required by x86 architecture */
// 	gdt_set_gate(0, 0, 0, 0, 0); // Set the first GDT entry (null descriptor).

// 	/* Code segment: base=0, limit=4GB, accessed from ring 0 */
// 	gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // Set the code segment descriptor.

// 	/* Data segment: base=0, limit=4GB, accessed from ring 0 */
// 	gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // Set the data segment descriptor.

// 	/* Load the new GDT using the gdt_flush function */
// 	gdt_flush();
// }