#pragma once

template <typename T>
class Iterator
{
	using ValueType = T;
	using Pointer = T*;
	using Reference = T&;

	Pointer m_ptr;

public:
	explicit Iterator(const Pointer ptr) : m_ptr(ptr) {}

	Reference operator*() const { return *m_ptr; } // Dereference
	Pointer	  operator->() const { return m_ptr; } // Value
	Iterator& operator++()						   // Prefix increment
	{
		++m_ptr;
		return *this;
	}
	Iterator& operator++(int) // Postfix increment
	{
		Iterator tmp = *this;
		++(*this);
		return tmp;
	}
	friend bool operator==(const Iterator& a, const Iterator& b) // Equals
	{
		return a.m_ptr == b.m_ptr;
	}
	friend bool operator!=(const Iterator& a, const Iterator& b) // Not Equals
	{
		return a.m_ptr != b.m_ptr;
	}
};