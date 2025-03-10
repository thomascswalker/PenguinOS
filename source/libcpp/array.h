#pragma once

#include <allocator.h>
#include <cstdlib.h>
#include <initializerlist.h>
#include <iterator.h>
#include <stdio.h>

template <typename T>
class Array
{
	using ValueType = T;
	using SizeType = size_t;

	using PointerType = T*;
	using ConstPointerType = const T*;
	using ReferenceType = T&;
	using ConstReferenceType = const T&;

	using IterType = Iterator<T>;
	using ConstIterType = const Iterator<T>;

private:
	PointerType			 m_data;
	SizeType			 m_size;
	SizeType			 m_capacity;
	Allocator<ValueType> m_allocator;

public:
	Array() : m_data(nullptr), m_size(0), m_capacity(0) {}
	Array(std::initializer_list<ValueType> data)
	{
		m_size = data.size();
		m_capacity = data.size();
		m_data = m_allocator.allocate(m_capacity);
		ConstPointerType p = data.begin();
		for (SizeType i = 0; i < m_size; i++)
		{
			new (&m_data[i]) ValueType(p[i]);
		}
	}
	~Array()
	{
		for (SizeType i = 0; i < m_size; i++)
		{
			m_data[i].~ValueType();
		}
		if (m_data)
		{
			m_allocator.deallocate(m_data, m_capacity);
		}
	}

	PointerType data() const { return m_data; }

	void add(ConstReferenceType value)
	{
		if (m_size == m_capacity)
		{
			grow();
		}
		new (&m_data[m_size]) ValueType(value);
		m_size++;
	}

	void add(ReferenceType& value)
	{
		if (m_size == m_capacity)
		{
			grow();
		}
		new (&m_data[m_size]) ValueType(std::move(value));
		m_size++;
	}

	void grow()
	{
		SizeType newCapacity = m_capacity == 0 ? 1 : m_capacity * 2;
		reserve(newCapacity);
	}

	void reserve(SizeType newCapacity)
	{
		if (newCapacity <= m_capacity)
		{
			return;
		}

		PointerType newData = m_allocator.allocate(newCapacity);

		for (SizeType i = 0; i < m_size; i++)
		{
			new (&newData[i]) ValueType(std::move(m_data[i]));
			m_data[i].~ValueType();
		}

		if (m_data)
		{
			m_allocator.deallocate(m_data, m_capacity);
		}

		m_data = newData;
		m_capacity = newCapacity;
	}
	SizeType size() const { return m_size; }
	bool	 empty() const { return m_size == 0; }

	PointerType at(SizeType index) { return &m_data[index]; }

	IterType	  begin() { return IterType(m_data); }
	IterType	  end() { return IterType(m_data + m_size); }
	ConstIterType begin() const { return ConstIterType(m_data); }
	ConstIterType end() const { return ConstIterType(m_data + m_size); }

	ReferenceType	   operator[](SizeType index) { return m_data[index]; }
	ConstReferenceType operator[](SizeType index) const { return m_data[index]; }
};
