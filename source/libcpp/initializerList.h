#pragma once

#include <ctype.h>

template <typename T>
class InitializerList
{
public:
	typedef T		 ValueType;
	typedef const T& Reference;
	typedef const T& ConstReference;
	typedef size_t	 SizeType;
	typedef const T* Iterator;
	typedef const T* ConstIterator;

private:
	Iterator m_begin;
	SizeType m_size;

public:
	constexpr InitializerList() noexcept : m_begin(nullptr), m_size(0) {}
	constexpr InitializerList(const T* b, SizeType s) noexcept : m_begin(b), m_size(s) {}
	constexpr SizeType		size() const noexcept { return m_size; }
	constexpr ConstIterator begin() const noexcept { return m_begin; }
	constexpr ConstIterator end() const noexcept { return m_begin + m_size; }
};
