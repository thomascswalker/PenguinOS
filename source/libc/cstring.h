#pragma once

#include <ctype.h>
#include <stdarg.h>

// 	Copies one buffer to another.
EXTERN void* memcpy(void* dest, void* source, size_t size);
// Fills a buffer with a repeated byte.
EXTERN void* memset(void* dest, uint8_t value, int32_t size);
// Fills a buffer with a repeated byte.
EXTERN uint16_t* wmemset(uint16_t* dest, uint16_t value, int32_t size);
// Returns the length of the string.
EXTERN size_t strlen(const char* string);
// Compares two strings.
EXTERN bool strcmp(const char* lhs, const char* rhs);
EXTERN bool strncmp(const char* lhs, const char* rhs, size_t count);
// Copies one string to another
EXTERN char* strcpy(char* dest, const char* source);
// Appends one string to another
EXTERN char*   strncpy(char* dest, const char* source, size_t count);
EXTERN char*   strcat(char* dest, const char* source);
EXTERN char*   strchr(const char* s, char c);
EXTERN int32_t strchri(const char* s, char c);
EXTERN char*   strrchr(const char* s, char c);
EXTERN int32_t strrchri(const char* s, char c);
EXTERN char*   strdup(const char* str);
EXTERN char*   strtok(char* str, const char delim);
EXTERN char	   toupper(char c);
EXTERN char	   tolower(char c);
// Reverses, in-place, the specified `string`.
void strrev(char* string);
void strapp(char* source, const char* string, char* dest);
void strprep(char* source, const char* string, char* dest);
void wtoc(char* dest, wchar_t* source, size_t length);