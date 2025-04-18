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
	using PtrType = ValueType*;
	using ConstPtrType = const ValueType*;
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
		PtrType ptr = static_cast<PtrType>(malloc(n * sizeof(ValueType)));
		return ptr;
	}

	// Deallocate memory.
	void deallocate(PtrType p, SizeType n) noexcept { free(p); }
};

template <typename ValueType, typename U>
bool operator==(const Allocator<ValueType>&, const Allocator<U>&) noexcept
{
	return true;
}

template <typename ValueType, typename U>
bool operator!=(const Allocator<ValueType>& a, const Allocator<U>& b) noexcept
{
	return !(a == b);
}
