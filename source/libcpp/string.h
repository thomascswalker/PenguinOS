#pragma once

#include <array.h>
#include <assert.h>
#include <cstring.h>
#include <math.h>
#include <memory.h>

template <typename T>
class BasicString
{
	using ValueType = T;
	using SizeType = size_t;

	using PointerType = ValueType*;
	using ConstPointerType = const ValueType*;

	using IterType = ValueType*;
	using ConstIterType = const ValueType*;

	PointerType			 m_data;
	SizeType			 m_size;
	SizeType			 m_capacity;
	Allocator<ValueType> m_allocator;

public:
	static const SizeType npos = -1;

public:
	/* Constructors */

	// Default constructor
	BasicString() : m_size(0), m_capacity(1) { m_data = m_allocator.allocate(1); }
	BasicString(const BasicString<ValueType>& other)
	{
		m_size = other.m_size;
		m_capacity = other.m_capacity;
		m_data = m_allocator.allocate(m_size);
		memcpy(m_data, other.m_data, m_size);
		m_data[m_size] = '\0';
	}
	// Assignment constructor
	BasicString<ValueType>& operator=(const BasicString<ValueType>& other)
	{
		if (this == &other)
		{
			return *this;
		}
		reserve(other.size());
		memcpy(m_data, other.data(), other.size());
		m_size = other.size();
		m_data[m_size] = '\0';
		return *this;
	}
	// Constructor from C-style string
	BasicString(const char* str)
	{
		if (strlen(str) == 1)
		{
			m_size = 0;
			m_data = nullptr;
			m_capacity = 1;
			return;
		}
		m_size = strlen(str);
		reserve(m_size + 1);
		m_data = m_allocator.allocate(m_size);
		for (SizeType i = 0; i < m_size; i++)
		{
			m_data[i] = str[i];
		}
		m_data[m_size] = '\0';
	}
	BasicString(const ValueType* str, SizeType count) : m_size(count), m_capacity(count + 1)
	{
		if (strlen(str) == 1)
		{
			m_size = 0;
			m_data = nullptr;
			m_capacity = 1;
			return;
		}
		reserve(m_size + 1);
		m_data = m_allocator.allocate(m_size);
		for (SizeType i = 0; i < m_size; i++)
		{
			m_data[i] = str[i];
		}
		m_data[m_size] = '\0';
	}
	BasicString(SizeType size) : m_size(size), m_capacity(size + 1)
	{
		m_data = m_allocator.allocate(size);
		memset(m_data, '\0', strlen(m_data) + 1);
	}
	BasicString(SizeType size, ValueType c) : m_size(size), m_capacity(size + 1)
	{
		m_data = m_allocator.allocate(size);
		memset(m_data, c, strlen(m_data));
		m_data[m_size] = '\0';
	}
	~BasicString() { m_allocator.deallocate(m_data, m_capacity); }

	/* Methods */

	ValueType* data() const { return m_data; }
	SizeType   size() const { return m_size; }
	SizeType   capacity() const { return m_capacity; }
	void	   reserve(SizeType newCapacity)
	{
		if (m_capacity > newCapacity)
		{
			return;
		}

		ValueType* newData = new ValueType[newCapacity];

		// Copy old data
		if (m_data != nullptr)
		{
			memcpy(newData, m_data, m_size);
			m_allocator.deallocate(m_data, m_size);
		}

		m_data = newData;
		m_capacity = newCapacity;
	}
	void erase(SizeType pos, SizeType count = npos)
	{
		SizeType offset = pos + count;
		if (pos > m_size)
		{
			return;
		}
		if (count > m_size - pos || count == npos)
		{
			count = m_size - pos;
		}

		for (SizeType i = pos; i + count < m_size + 1; i++)
		{
			m_data[i] = m_data[i + count];
		}
		m_size -= count;
		m_data[m_size] = '\0';
	}
	void append(ValueType c)
	{
		if (m_size == m_capacity)
		{
			SizeType newCapacity = m_capacity == 0 ? 1 : m_capacity * 2;
			reserve(newCapacity);
		}
		m_data[m_size] = c;
		m_size++;
	}
	void append(const BasicString<ValueType>& p)
	{
		for (ValueType c : p)
		{
			append(c);
		}
	}
	Array<BasicString<ValueType>> split(ValueType delimiter) const
	{
		Array<BasicString<ValueType>> tokens;

		SizeType start = 0;
		SizeType end = find(delimiter);

		while (end != npos)
		{
			SizeType diff = end - start;
			if (diff)
			{
				BasicString<ValueType> s = substr(start, diff);
				tokens.add(s);
			}
			start = end + 1; // Skip delimiter
			end = find(delimiter, start);
		}

		end = m_size;
		tokens.add(substr(start, end - start));

		// for (auto& t : tokens)
		// {
		// 	t.m_data[t.m_size] = '\0';
		// }

		return tokens;
	}
	void resize(SizeType size, ValueType c = '\0')
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
				ValueType* newData = new ValueType[m_capacity + 1];
				memcpy(newData, m_data, m_size);
				for (SizeType i = m_size; i < size; i++)
				{
					newData[i] = c;
				}
				delete[] m_data;
				m_data = newData;
			}
			else
			{
				for (SizeType i = m_size; i < size; i++)
				{
					m_data[i] = c;
				}
				m_data[size] = '\0';
			}
			m_size = size;
		}
		m_data[m_size] = '\0';
	}
	BasicString<ValueType> substr(SizeType pos, SizeType count = BasicString<ValueType>::npos) const
	{
		count = (count == npos || pos + count > m_size) ? m_size - pos : count;
		BasicString<ValueType> result(m_data + pos, count);
		return result;
	}

	SizeType find(ValueType c, SizeType start = 0) const
	{
		for (SizeType i = start; i < m_size; i++)
		{
			if (m_data[i] == c)
			{
				return i;
			}
		}
		return BasicString<ValueType>::npos;
	}
	SizeType findFirst(ValueType c) const { return find(c, 0); }
	SizeType findLast(ValueType c, SizeType pos = BasicString<ValueType>::npos) const
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

		for (SizeType i = pos; i > 0; i--)
		{
			if (m_data[i] == c)
			{
				return i;
			}
		}

		return m_data[0] == c ? 0 : npos;
	}

	bool startsWith(char c) const { return m_data[0] == c; }
	bool endsWith(char c) const { return m_data[m_size - 1] == c; }
	bool startsWith(const char* str) const { return strncmp(m_data, str, strlen(str)); }
	bool endsWith(const char* str) const
	{
		size_t i = m_size - strlen(str);
		return strncmp(m_data + i, str, strlen(str));
	}

	bool empty() const { return m_size == 0; }
	void clear()
	{
		m_size = 0;
		m_capacity = 1;
		m_data = m_allocator.allocate(1);
	}
	void trim(ValueType c)
	{
		int pos = m_size - 1;
		while (m_data[pos] == c)
		{
			m_data[pos] = '\0';
			m_size--;
		}
	}
	void trimWhitespace()
	{
		trim(' ');
		trim('\n');
		trim('\t');
	}

	ValueType* cstr() const
	{
		ValueType* result = (ValueType*)malloc(m_size);
		result[m_size + 1] = '\0';
		return result;
	}

	IterType	  begin() { return IterType(m_data); }
	ConstIterType begin() const { return ConstIterType(m_data); }
	IterType	  end() { return IterType(m_data + m_size); }
	ConstIterType end() const { return ConstIterType(m_data + m_size); }
	/* Operators */

	BasicString<ValueType> operator+(const BasicString<ValueType>& other)
	{
		BasicString<ValueType> temp(*this);
		for (ValueType c : other)
		{
			temp.append(c);
		}
		temp[temp.m_size] = '\0';
		return temp;
	}
	BasicString<ValueType>& operator+=(const BasicString<ValueType>& other)
	{
		for (ValueType c : other)
		{
			append(c);
		}
		m_data[m_size] = '\0';
		return *this;
	}
	BasicString<ValueType> operator+(const ValueType* other)
	{
		BasicString<ValueType> temp = *this;
		for (uint32_t i = 0; i < strlen(other); i++)
		{
			temp.append(other[i]);
		}
		temp[temp.m_size] = '\0';
		return temp;
	}
	BasicString<ValueType>& operator+=(const ValueType* other)
	{
		*this = *this + other;
		return *this;
	}
	bool operator==(const BasicString<ValueType>& other) const
	{
		return strcmp(m_data, other.data());
	}
	bool operator==(ValueType* other) const { return strcmp(m_data, other); }
	bool operator==(const ValueType* other) const { return strcmp(m_data, other); }
	bool operator!=(ValueType* other) const { return !(*this == other); }
	bool operator!=(const ValueType* other) const { return !(*this == other); }
	bool operator!=(const BasicString<ValueType>& other) const { return !(*this == other); }
	operator ValueType*() const { return m_data; }
	ValueType&		 operator[](SizeType index) { return m_data[index]; }
	const ValueType& operator[](SizeType index) const { return m_data[index]; }
};

template <typename ValueType>
static BasicString<ValueType> operator+(const ValueType* lhs, const BasicString<ValueType>& rhs)
{
	BasicString<ValueType> result(lhs);
	for (auto c : rhs)
	{
		result.append(c);
	}
	return result;
}

typedef BasicString<char>	 String;
typedef BasicString<wchar_t> WideString;