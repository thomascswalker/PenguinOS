#pragma once

#include <allocator.h>
#include <initializerlist.h>
#include <stdio.h>
#include <stdlib.h>

template <typename T>
class Array
{
	using Iterator = T*;
	using ConstIterator = const T*;

	T*			 m_data;
	size_t		 m_size;
	size_t		 m_capacity;
	Allocator<T> m_allocator;

public:
	Array() : m_data(nullptr), m_size(0), m_capacity(0) {}
	Array(InitializerList<T> data)
	{
		m_size = data.size();
		m_capacity = data.size();
		m_data = m_allocator.allocate(m_capacity);
		const T* p = data.begin();
		for (uint32_t i = 0; i < m_size; i++)
		{
			new (&m_data[i]) T(p[i]);
		}
	}
	~Array()
	{
		for (size_t i = 0; i < m_size; i++)
		{
			m_data[i].~T();
		}
		if (m_data)
		{
			m_allocator.deallocate(m_data, m_capacity);
		}
	}

	void add(const T& value)
	{
		if (m_size == m_capacity)
		{
			uint32_t newCapacity = m_capacity == 0 ? 1 : m_capacity * 2;
			reserve(newCapacity);
		}
		new (&m_data[m_size]) T(std::move(value));
		m_size++;
	}

	void reserve(size_t newCapacity)
	{
		if (newCapacity <= m_capacity)
		{
			return;
		}

		T* newData = m_allocator.allocate(newCapacity);

		for (size_t i = 0; i < m_size; i++)
		{
			newData[i] = T(std::move(m_data[i]));
			m_data[i].~T();
		}

		if (m_data)
		{
			m_allocator.deallocate(m_data, m_capacity);
		}

		m_data = newData;
		m_capacity = newCapacity;
	}
	size_t size() const { return m_size; }
	bool   empty() const { return m_size == 0; }

	Iterator	  begin() { return m_data; }
	Iterator	  end() { return m_data + m_size; }
	ConstIterator begin() const { return m_data; }
	ConstIterator end() const { return m_data + m_size; }

	T&		 operator[](size_t index) { return m_data[index]; }
	const T& operator[](size_t index) const { return m_data[index]; }
};
