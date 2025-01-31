#pragma once

#include <ctype.h>
#include <stdint.h>

#define BLOCK_SIZE 4096
#define BITS_PER_BYTE 8

void*	 kmalloc(uint32_t size);
void*	 ksbrk(uint32_t increment);
uint32_t kbrk(void* end_data_segment);

namespace Memory
{
	struct Block
	{
		uint32_t size;
		bool	 present;
		Block*	 next;

		Block(uint32_t inSize, bool inPresent) : present(inPresent), next(nullptr)
		{
			size = ALIGN(inSize, BLOCK_SIZE);
		}
	} __attribute__((aligned(16)));

	void  init(uint32_t address, uint32_t size);
	void* allocate(uint32_t size);
} // namespace Memory