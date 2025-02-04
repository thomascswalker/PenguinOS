#pragma once

#include <stddef.h>
#include <stdint.h>

#define EXTERN extern "C"
#define ALIGN(x, n) (((uint32_t)(x) + (n) - 1) & ~((n) - 1))

static bool isascii(int32_t c) { return ((c >= 0) && (c <= 255)); }
static bool isintegral(int32_t c) { return ((c >= 48) && (c <= 57)); }
