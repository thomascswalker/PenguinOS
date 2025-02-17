#pragma once

#include <stddef.h>
#include <stdint.h>

#define EXTERN extern "C"
#define ALIGN(x, n) (((uint32_t)(x) + (n) - 1) & ~((n) - 1))

static bool isascii(unsigned char c) { return c <= 127; }
static bool isintegral(unsigned char c) { return ((c >= 48) && (c <= 57)); }
static bool isalpha(unsigned char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'); }
static bool isdigit(unsigned char c) { return (c >= '0' && c <= '9'); }
static bool isalnum(unsigned char c) { return (isalpha(c) || isdigit(c)); }
static bool islower(unsigned char c) { return (c >= 'a' && c <= 'z'); }
static bool isupper(unsigned char c) { return (c >= 'A' && c <= 'Z'); }

static void wctoc(wchar_t* in, char* out, uint32_t count)
{
	for (uint32_t i = 0; i < count; i++)
	{
		out[i] = (char)in[i];
	}
}