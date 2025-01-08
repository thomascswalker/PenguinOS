#pragma once

#include <memory.h>
#include <stdint.h>

void init_kmalloc(uint32_t initial_heap_size);
void change_heap_size(uint32_t new_size);