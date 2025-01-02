#pragma once

#include <string.h>

#define NULL 0
#define nullptr ((void*)0)

void itoa(char* buffer, uint32_t value, uint32_t base)
{
	uint32_t temp = value;
	uint32_t i = 0;

	do
	{
		temp = value % base;
		buffer[i++] = (temp < 10) ? (temp + '0') : (temp + 'a' - 10);
	}
	while (value /= base);

	buffer[i--] = 0;

	for (uint32_t j = 0; j < i; j++, i--)
	{
		temp = buffer[j];
		buffer[j] = buffer[i];
		buffer[i] = temp;
	}
}