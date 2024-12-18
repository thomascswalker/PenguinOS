#include "types.h"

// Copy `size` of memory from `source` to `destination`.
uint8* memcpy(uint8* dest, uint8* source, size_t size)
{
	uint8*		 p_dest = (uint8*)dest;
	const uint8* p_source = (uint8*)source;

	while (size--)
	{
		*p_dest++ = *p_source++;
	}
	return dest;
}

// Set `size` bytes in `dest` to `val`. (8-bit)
uint8* memset(uint8* dest, uint8 value, int32 size)
{
	for (int32 i = 0; i < size; i++)
	{
		dest[i] = value;
	}
	return dest;
}

// Set `size` bytes in `dest` to `val`. (16-bit)
uint16* memsetw(uint16* dest, uint16 value, int32 size)
{
	for (int32 i = 0; i < size; i++)
	{
		dest[i] = value;
	}
	return dest;
}