#pragma once

#include <ctype.h>
#include <stdio.h>

void* __dso_handle = nullptr;

using ConstructorType = void (*)();

// These symbols must be defined in your linker script.
EXTERN ConstructorType __ctor_start[];
EXTERN ConstructorType __ctor_end[];

EXTERN void callConstructors()
{
	debug("Calling constructors...");
	for (ConstructorType* ctor = __ctor_start; ctor < __ctor_end; ++ctor)
	{
		(*ctor)();
	}
	debug("Done with constructors...");
}

EXTERN int __cxa_atexit(void (*func)(void*), void* arg, void* dso_handle)
{
	// In a typical OS kernel, destructors might not be needed.
	// Simply return 0 to indicate success.
	return 0;
}
