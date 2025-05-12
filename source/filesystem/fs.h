#pragma once

#include "bitmask.h"

#include <stdint.h>

#define MAX_FILENAME_LENGTH 256

enum INodeType
{
	IT_Block,
	IT_Directory,
	IT_Regular,
	IT_Unknown
};

// Represents a file in the filesystem
struct INode
{
	char	  name[MAX_FILENAME_LENGTH]; // INode name
	size_t	  size;						 // Size of the buffer
	int32_t	  fd;						 // INode descriptor
	INodeType type;						 // Type of the INode

	bool isDirectory() const { return Bitmask::test(type, IT_Directory); }
};

// Represents an open file
struct File
{
	INode* inode;  // Pointer to the INode
	char*  buffer; // Buffer for file I/O
	size_t pos;	   // Current position in the file

	~File() { delete[] buffer; }
};

struct FileStat
{
	size_t size; // Size of the file
};