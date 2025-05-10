#pragma once

#include <stdint.h>

struct File
{
	char*	name;		 // File name
	char*	buffer;		 // Buffer for file I/O
	size_t	size;		 // Size of the buffer
	int32_t fd;			 // File descriptor
	bool	isDirectory; // Is this a directory?
	size_t	pos;		 // Current position in the file

	~File()
	{
		delete[] buffer;
		delete[] name;
	}

	bool isValid() const { return fd > 0; }
};

struct FileStat
{
	size_t size; // Size of the file
};