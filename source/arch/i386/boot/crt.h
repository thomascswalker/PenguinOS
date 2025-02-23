#pragma once

extern "C" void* __dso_handle = nullptr;

using ConstructorType = void (*)();

// These symbols must be defined in your linker script.
extern "C" ConstructorType __ctor_start[];
extern "C" ConstructorType __ctor_end[];

extern "C" void callConstructors()
{
	for (ConstructorType* ctor = __ctor_start; ctor < __ctor_end; ++ctor)
	{
		(*ctor)();
	}
}

extern "C" int __cxa_atexit(void (*func)(void*), void* arg, void* dso_handle)
{
	// In a typical OS kernel, destructors might not be needed.
	// Simply return 0 to indicate success.
	return 0;
}
