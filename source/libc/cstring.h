#pragma once

#include <ctype.h>
#include <stdarg.h>

// 	Copies one buffer to another.
void* memcpy(void* dest, void* source, size_t size);
// Fills a buffer with a repeated byte.
void* memset(void* dest, uint8_t value, int32_t size);
// Fills a buffer with a repeated byte.
uint16_t* wmemset(uint16_t* dest, uint16_t value, int32_t size);
// Returns the length of the string.
size_t strlen(const char* string);
// Compares two strings.
bool strcmp(const char* lhs, const char* rhs);
bool strncmp(const char* lhs, const char* rhs, size_t count);
// Copies one string to another
char* strcpy(char* dest, const char* source);
// Appends one string to another
char*	strncpy(char* dest, const char* source, size_t count);
char*	strcat(char* dest, const char* source);
char*	strchr(const char* s, char c);
int32_t strchri(const char* s, char c);
char*	strrchr(const char* s, char c);
int32_t strrchri(const char* s, char c);
char*	strdup(const char* str);
char*	strtok(char* str, const char delim);
char	toupper(char c);
char	tolower(char c);

// Reverses, in-place, the specified `string`.
void strrev(char* string);
void wtoc(char* dest, wchar_t* source, size_t length);

bool startswith(const char* str, const char* prefix);
bool endswith(const char* str, const char* suffix);