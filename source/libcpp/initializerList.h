#pragma once

#include <ctype.h>

namespace std
{
	template <typename T>
	class initializer_list
	{
	public:
		typedef T		 value_type;
		typedef const T& reference;
		typedef const T& const_reference;
		typedef size_t	 size_type;
		typedef const T* iterator;
		typedef const T* const_iterator;

	private:
		iterator  _begin;
		size_type _size;

	public:
		// Default constructor
		constexpr initializer_list() noexcept : _begin(nullptr), _size(0) {}

		// Constructor used by the compiler to create initializer_list objects
		constexpr initializer_list(const T* b, size_type s) noexcept : _begin(b), _size(s) {}

		// Accessors
		constexpr size_type		 size() const noexcept { return _size; }
		constexpr const_iterator begin() const noexcept { return _begin; }
		constexpr const_iterator end() const noexcept { return _begin + _size; }
	};
}