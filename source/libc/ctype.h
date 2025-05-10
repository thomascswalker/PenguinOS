#pragma once

#include <cstddef>
#include <stdint.h>

#define EXTERN extern "C"
#define ALIGN(x, n) (((uint32_t)(x) + (n) - 1) & ~((n) - 1))


static bool isascii(const uint8_t c) { return c <= 127; }
static bool isintegral(const uint8_t c) { return ((c >= 48) && (c <= 57)); }
static bool isalpha(const uint8_t c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'); }
static bool isdigit(const uint8_t c) { return (c >= '0' && c <= '9'); }
static bool isalnum(const uint8_t c) { return (isalpha(c) || isdigit(c)); }
static bool islower(const uint8_t c) { return (c >= 'a' && c <= 'z'); }
static bool isupper(const uint8_t c) { return (c >= 'A' && c <= 'Z'); }

static void wctoc(const wchar_t* in, char* out, const uint32_t count)
{
	for (uint32_t i = 0; i < count; i++)
	{
		out[i] = static_cast<char>(in[i]);
	}
}