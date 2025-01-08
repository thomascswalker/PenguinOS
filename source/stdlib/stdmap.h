#pragma once

typedef struct hashmap_entry
{
	char*				  key;
	void*				  value;
	struct hashmap_entry* next;
} hashmap_entry_t;

typedef struct hashmap
{
	size_t			  size;
	hashmap_entry_t** entries;
} hashmap_t;
