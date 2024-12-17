#include "types.h"

void* mem_copy(void* dest, void* source, size_t size)
{
	char*		p_dest = (char*)dest;
	const char* p_source = (char*)source;

	while (size--)
	{
		*p_dest++ = *p_source++;
	}
	return dest;
}