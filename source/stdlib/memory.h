#include <types.h>

// Copy `size` of memory from `source` to `destination`. (8-bit)
void* memcpy(void* dest, void* source, size_t size)
{
	uint8_t*	   p_dest = (uint8_t*)dest;
	const uint8_t* p_source = (uint8_t*)source;

	while (size--)
	{
		*p_dest++ = *p_source++;
	}
	return dest;
}

// Set `size` bytes in `dest` to `val`. (8-bit)
uint8_t* memset(uint8_t* dest, uint8_t value, int32_t size)
{
	for (int32_t i = 0; i < size; i++)
	{
		dest[i] = value;
	}
	return dest;
}

// Set `size` bytes in `dest` to `val`. (16-bit)
uint16_t* memsetw(uint16_t* dest, uint16_t value, int32_t size)
{
	for (int32_t i = 0; i < size; i++)
	{
		dest[i] = value;
	}
	return dest;
}