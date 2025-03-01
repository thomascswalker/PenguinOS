#pragma once

#include <ctype.h>

namespace std
{
	template <typename T>
	class initializer_list
	{
	public:
		typedef T		 ValueType;
		typedef const T& Reference;
		typedef const T& ConstReference;
		typedef size_t	 SizeType;
		typedef const T* IterType;
		typedef const T* ConstIterType;

	private:
		IterType m_begin;
		SizeType m_size;

	public:
		constexpr initializer_list() noexcept : m_begin(nullptr), m_size(0) {}
		constexpr initializer_list(const T* b, SizeType s) noexcept : m_begin(b), m_size(s) {}
		constexpr SizeType		size() const noexcept { return m_size; }
		constexpr ConstIterType begin() const noexcept { return m_begin; }
		constexpr ConstIterType end() const noexcept { return m_begin + m_size; }
	};
} // namespace std