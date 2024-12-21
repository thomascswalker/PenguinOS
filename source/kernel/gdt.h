// https://www.thejat.in/learn/implement-gdt-in-multiboot-bootloader

#include <system.h>

#define i686_GDT_CODE_SEGMENT 0x08
#define i686_GDT_DATA_SEGMENT 0x10

struct gdt_entry
{
	uint16 limit_low;
	uint16 base_low;
	uint8  base_middle;
	uint8  access;
	uint8  granularity;
	uint8  base_high;
} __attribute__((packed));
typedef struct gdt_entry gdt_entry_t;

struct gdt_desc
{
	uint16 limit;
	uint32 base;
} __attribute__((packed));
typedef struct gdt_desc gdt_desc_t;

typedef enum
{
	GDT_ACCESS_CODE_READABLE = 0x02,
	GDT_ACCESS_DATA_WRITEABLE = 0x02,

	GDT_ACCESS_CODE_CONFORMING = 0x04,
	GDT_ACCESS_DATA_DIRECTION_NORMAL = 0x00,
	GDT_ACCESS_DATA_DIRECTION_DOWN = 0x04,

	GDT_ACCESS_DATA_SEGMENT = 0x10,
	GDT_ACCESS_CODE_SEGMENT = 0x18,

	GDT_ACCESS_DESCRIPTOR_TSS = 0x00,

	GDT_ACCESS_RING0 = 0x00,
	GDT_ACCESS_RING1 = 0x20,
	GDT_ACCESS_RING2 = 0x40,
	GDT_ACCESS_RING3 = 0x60,

	GDT_ACCESS_PRESENT = 0x80,

} GDT_ACCESS;

typedef enum
{
	GDT_FLAG_64BIT = 0x20,
	GDT_FLAG_32BIT = 0x40,
	GDT_FLAG_16BIT = 0x00,

	GDT_FLAG_GRANULARITY_1B = 0x00,
	GDT_FLAG_GRANULARITY_4K = 0x80,
} GDT_FLAGS;

#define GDT_LIMIT_LOW(limit) ((limit) & 0xFFFF)
#define GDT_BASE_LOW(base) ((base) & 0xFFFF)
#define GDT_BASE_MIDDLE(base) (((base) >> 16) & 0xFF)
#define GDT_FLAGS_LIMIT_HI(limit, flags) ((((limit) >> 16) & 0xF) | ((flags) & 0xF0))
#define GDT_BASE_HIGH(base) (((base) >> 24) & 0xFF)

#define GDT_ENTRY(base, limit, access, flags) { GDT_LIMIT_LOW(limit), GDT_BASE_LOW(base), GDT_BASE_MIDDLE(base), access, GDT_FLAGS_LIMIT_HI(limit, flags), GDT_BASE_HIGH(base) }

gdt_entry_t g_gdt[] = {
	// NULL descriptor
	GDT_ENTRY(0, 0, 0, 0),

	// Kernel 32-bit code segment
	GDT_ENTRY(0, 0xFFFFF, GDT_ACCESS_PRESENT | GDT_ACCESS_RING0 | GDT_ACCESS_CODE_SEGMENT | GDT_ACCESS_CODE_READABLE, GDT_FLAG_32BIT | GDT_FLAG_GRANULARITY_4K),

	// Kernel 32-bit data segment
	GDT_ENTRY(0, 0xFFFFF, GDT_ACCESS_PRESENT | GDT_ACCESS_RING0 | GDT_ACCESS_DATA_SEGMENT | GDT_ACCESS_DATA_WRITEABLE, GDT_FLAG_32BIT | GDT_FLAG_GRANULARITY_4K),

};

gdt_desc_t g_gdt_desc = { sizeof(gdt_entry_t) - 1, (uint32)g_gdt };

// Defined in gdt.asm
// Reloads the segment registers.
extern void gdt_flush(gdt_desc_t* descriptor, uint16 code, uint16 data);

void gdt_install()
{
	gdt_flush(&g_gdt_desc, i686_GDT_CODE_SEGMENT, i686_GDT_DATA_SEGMENT);
}
