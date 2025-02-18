#pragma once

#include <array.h>
#include <assert.h>
#include <cstring.h>
#include <math.h>
#include <memory.h>

class String
{
	using Iterator = char*;
	using ConstIterator = const char*;

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
		m_data = m_allocator.allocate(m_capacity);
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
	String(const char* str, size_t count) : m_size(count)
	{
		m_data = new char[count + 1];
		memcpy(m_data, (char*)str, count);
		m_data[count] = '\0';
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
	size_t capacity() const { return m_capacity; }
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
		m_data[m_size] = c;
		m_size++;
	}
	Array<String> split(char delimiter) const
	{
		Array<String> tokens;

		size_t start = 0;
		size_t end = find(delimiter);

		while (end != npos)
		{
			size_t diff = end - start;
			if (diff)
			{
				String s = substr(start, diff);
				tokens.add(s);
			}
			start = end + 1; // Skip delimiter
			end = find(delimiter, start);
		}

		end = m_size;
		tokens.add(substr(start, end - start));

		return tokens;
	}
	void resize(size_t size, char c = '\0')
	{
		if (size < m_size)
		{
			m_size = size;
		}
		else if (size > m_size)
		{
			if (size > m_capacity)
			{
				m_capacity = size;
				char* newData = new char[m_capacity + 1];
				memcpy(newData, m_data, m_size);
				for (size_t i = m_size; i < size; i++)
				{
					newData[i] = c;
				}
				delete[] m_data;
				m_data = newData;
			}
			else
			{
				for (size_t i = m_size; i < size; i++)
				{
					m_data[i] = c;
				}
				m_data[size] = '\0';
			}
			m_size = size;
		}
		m_data[m_size] = '\0';
	}
	String substr(size_t pos, size_t count = String::npos) const
	{
		count = (count == npos || pos + count > m_size) ? m_size - pos : count;
		return String(m_data + pos, count);
	}

	size_t find(char c, size_t start = 0) const
	{
		for (size_t i = start; i < m_size; i++)
		{
			if (m_data[i] == c)
			{
				return i;
			}
		}
		return String::npos;
	}
	size_t findFirst(char c) const { return find(c, 0); }
	size_t findLast(char c, size_t pos = String::npos) const
	{
		if (m_size == 0)
		{
			return npos;
		}

		// If pos is npos or beyond the end of the string, start from the last character.
		if (pos >= m_size)
		{
			pos = m_size - 1;
		}

		for (size_t i = pos; i > 0; i--)
		{
			if (m_data[i] == c)
			{
				return i;
			}
		}

		return m_data[0] == c ? 0 : npos;
	}
	bool empty() const { return m_size != 0; }
	void clear()
	{
		m_size = 0;
		m_capacity = 1;
		m_data = m_allocator.allocate(1);
	}

	char* cstr() const { return m_data; }

	Iterator	  begin() { return m_data; }
	ConstIterator begin() const { return m_data; }
	Iterator	  end() { return m_data + m_size; }
	ConstIterator end() const { return m_data + m_size; }
	/* Operators */

	String operator+(const String& other)
	{
		String temp(*this);
		for (char c : other)
		{
			temp.append(c);
		}
		temp[temp.m_size] = '\0';
		return temp;
	}
	String& operator+=(const String& other)
	{
		for (char c : other)
		{
			append(c);
		}
		m_data[m_size] = '\0';
		return *this;
	}
	String operator+(const char* other)
	{
		String temp = *this;
		for (uint32_t i = 0; i < strlen(other); i++)
		{
			temp.append(other[i]);
		}
		temp[temp.m_size] = '\0';
		return temp;
	}
	String& operator+=(const char* other)
	{
		*this = *this + other;
		return *this;
	}
	bool operator==(const String& other)
	{
		if (m_size != other.size())
		{
			return false;
		}
		for (size_t i = 0; i < m_size; i++)
		{
			if (m_data[i] != other[i])
			{
				return false;
			}
		}
		return true;
	}
	bool operator!=(const String& other) { return !(*this == other); }
	operator char*() const { return m_data; }
	char&		operator[](size_t index) { return m_data[index]; }
	const char& operator[](size_t index) const { return m_data[index]; }
};

static String operator+(const char* lhs, const String& rhs)
{
	String result(lhs);
	for (auto c : rhs)
	{
		result.append(c);
	}
	return result;
}