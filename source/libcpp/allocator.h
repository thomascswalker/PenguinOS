#pragma once

#include <cstdlib.h>
#include <stddef.h>
#include <stdint.h>

// A simple allocator that uses malloc and free
template <typename T>
class Allocator
{
public:
	using ValueType = T;
	using PtrType = T*;
	using ConstPtrType = const T*;
	using SizeType = size_t;
	using DiffType = ptrdiff_t;

	Allocator() noexcept {}

	template <typename U>
	Allocator(const Allocator<U>&) noexcept
	{
	}

	// Allocate memory for n objects of type T.
	PtrType allocate(SizeType n)
	{
		PtrType ptr = static_cast<PtrType>(std::malloc(n * sizeof(T)));
		return ptr;
	}

	// Deallocate memory.
	void deallocate(PtrType p, SizeType n) noexcept { std::free(p); }
};

template <typename T, typename U>
bool operator==(const Allocator<T>&, const Allocator<U>&) noexcept
{
	return true;
}

template <typename T, typename U>
bool operator!=(const Allocator<T>& a, const Allocator<U>& b) noexcept
{
	return !(a == b);
}
