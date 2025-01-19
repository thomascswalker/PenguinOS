#pragma once

#include <string.h>

#define NULL 0

void  itoa(char* buffer, uint32_t value, uint32_t base);
void* malloc(const uint32_t size);
void  free(const void* ptr);