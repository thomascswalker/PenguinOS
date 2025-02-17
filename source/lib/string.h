#pragma once

#include <array.h>
#include <assert.h>
#include <cstring.h>
#include <math.h>
#include <memory.h>

class String
{
private:
	char*			m_data;
	size_t			m_size;
	size_t			m_capacity;
	Allocator<char> m_allocator;

public:
	static const size_t npos = -1;

public:
	/* Constructors */

	// Default constructor
	String() : m_size(0), m_capacity(1)
	{
		m_data = m_allocator.allocate(m_size);
		m_data[0] = '\0';
	}
	String(const String& other)
	{
		m_size = other.m_size;
		m_capacity = other.m_capacity;
		m_data = m_allocator.allocate(m_size);
		memcpy(m_data, other.m_data, m_size);
	}
	String& operator=(const String& other)
	{
		if (this == &other)
		{
			return *this;
		}
		String tmp(other);
		std::swap(m_data, tmp.m_data);
		std::swap(m_size, tmp.m_size);
		std::swap(m_capacity, tmp.m_capacity);
		return *this;
	}
	// Constructor from C-style string
	String(const char* str)
	{
		if (str == nullptr || !strlen(str))
		{
			m_size = 0;
			m_capacity = 0;
			m_data = m_allocator.allocate(m_capacity);
			return;
		}

		m_size = strlen(str);
		reserve(m_size + 1);
		m_data = m_allocator.allocate(m_size);
		for (size_t i = 0; i < m_size; i++)
		{
			m_data[i] = str[i];
		}
	}
	String(size_t size) : m_size(size), m_capacity(size + 1)
	{
		m_data = m_allocator.allocate(size);
		memset(m_data, '\0', strlen(m_data) + 1);
	}
	String(size_t size, char c) : m_size(size), m_capacity(size + 1)
	{
		m_data = m_allocator.allocate(size);
		memset(m_data, c, strlen(m_data));
	}
	~String()
	{
		if (m_data)
		{
			m_allocator.deallocate(m_data, m_capacity);
		}
	}

	/* Methods */

	char*  data() const { return m_data; }
	size_t size() const { return m_size; }
	void   reserve(size_t newCapacity)
	{
		if (m_capacity > newCapacity)
		{
			return;
		}

		char* newData = new char[newCapacity];
		if (m_data != nullptr)
		{
			memcpy(newData, m_data, m_size);
			delete[] m_data;
		}

		m_data = newData;
		m_capacity = newCapacity;
	}
	void append(char c)
	{
		if (m_size == m_capacity)
		{
			size_t newCapacity = m_capacity == 0 ? 1 : m_capacity * 2;
			reserve(newCapacity);
		}
		m_data[m_size - 1] = c;
		m_size++;
	}
	Array<String> split(char delimiter)
	{
		Array<String> tokens;

		size_t start = 0;
		size_t end = find(delimiter);

		while (end != npos)
		{
			if (end - start)
			{
				tokens.add(substring(start, end - start));
			}
			start = end + 1;
			end = find(delimiter, start);
		}

		end = m_size;
		tokens.add(substring(start, end - start));

		return tokens;
	}
	Array<String> split(char delimiter) const { return split(delimiter); }

	String substring(size_t pos, size_t count)
	{
		if (pos + count > m_size)
		{
			error("Out of range (%d > %d)", pos, m_size);
			return String();
		}

		String result;

		size_t length = std::min(count, m_size - pos);
		for (size_t i = pos - 1; i < pos + length; i++)
		{
			auto c = m_data[i];
			result.append(c);
		}
		return result;
	}
	String substring(size_t pos, size_t count) const { return substring(pos, count); }
	size_t find(char c, size_t start = 0)
	{
		for (size_t i = start; i < size(); i++)
		{
			if (m_data[i] == c)
			{
				return i;
			}
		}
		return String::npos;
	}

	bool empty() const { return m_size != 0; }
	void clear()
	{
		m_size = 0;
		m_capacity = 1;
		m_data = m_allocator.allocate(1);
	}

	char* cstr() const { return m_data; }

	/* Operators */

	operator char*() const { return m_data; }
};
