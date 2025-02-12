#pragma once

#include <stddef.h>
#include <stdint.h>

#define EXTERN extern "C"
#define ALIGN(x, n) (((uint32_t)(x) + (n) - 1) & ~((n) - 1))

static bool isascii(char c) { return ((c >= 0) && (c <= 127)); }
static bool isintegral(char c) { return ((c >= 48) && (c <= 57)); }
static bool isalpha(char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'); }
static bool islower(char c) { return (c >= 'a' && c <= 'z'); }
static bool isupper(char c) { return (c >= 'A' && c <= 'Z'); }

static void wctoc(wchar_t* in, char* out, uint32_t count)
{
	for (uint32_t i = 0; i < count; i++)
	{
		out[i] = (char)in[i];
	}
}