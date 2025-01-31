#pragma once

#include <stdio.h>

template <typename T>
class BitArray
{
	T*		 m_data = nullptr;
	size_t	 m_size = 0;
	uint32_t m_bitSize = 0;

public:
	BitArray(size_t size = 0)
	{
		m_size = size;
		m_bitSize = m_size * sizeof(T) * 8;
		memset(m_data, 0, m_size * sizeof(T));
	}

	T* data() const
	{
		return m_data;
	}

	size_t size() const
	{
		return m_size;
	}

	size_t bitSize() const
	{
		return m_bitSize;
	}

	void set(size_t pos)
	{
		size_t	index = pos == 0 ? 0 : pos / (sizeof(T) * 8);
		uint8_t offset = pos == 0 ? 0 : pos % (sizeof(T) * 8);
		m_data[index] |= (1 << offset);
	}

	void reset(size_t pos)
	{
		size_t	index = pos == 0 ? 0 : pos / (sizeof(T) * 8);
		uint8_t offset = pos == 0 ? 0 : pos % (sizeof(T) * 8);
		m_data[index] &= ~(1 << offset);
	}

	void fill(bool state)
	{
		T value = state ? 0xFFFFFFFF : 0x0;
		memset(m_data, value, m_size);
	}

	void fill(bool state, uint32_t pos, uint32_t size)
	{
		T value = state ? 0xFFFFFFFF : 0x0;
		memset(m_data + pos, value, size / sizeof(T));
	}

	void clear()
	{
		memset(m_data, 0, m_size);
	}

	// Tests a single index within the bitarray, returning true
	// if it is 1 and false if it is 0.
	bool test(size_t pos) const
	{
		size_t	index = pos == 0 ? 0 : pos / (sizeof(T) * 8);
		uint8_t offset = pos == 0 ? 0 : pos % (sizeof(T) * 8);
		return (m_data[index] & (1 << offset)) != 0;
	}

	bool all() const
	{
		for (uint32_t i = 0; i < m_size; i++)
		{
			if (!test(i))
			{
				return false;
			}
		}
		return true;
	}

	bool all(uint32_t pos, uint32_t size)
	{
		for (uint32_t i = pos; i < pos + size; i++)
		{
			if (!test(i))
			{
				return false;
			}
		}
		return true;
	}

	bool any() const
	{
		for (uint32_t i = 0; i < m_size; i++)
		{
			if (test(i))
			{
				return true;
			}
		}
		return false;
	}

	bool any(uint32_t pos, uint32_t size) const
	{
		for (uint32_t i = pos; i < pos + size; i++)
		{
			if (test(i))
			{
				return true;
			}
		}
		return false;
	}

	bool empty() const
	{
		return !any();
	}

	int32_t firstFree() const
	{
		for (size_t i = 0; i < m_size; i++)
		{
			if (m_data[i] == 0xFFFFFFFF)
			{
				continue;
			}
			for (size_t j = 0; j < 32; j++)
			{
				size_t offset = i * 32 + j;
				if (!(test(offset)))
				{
					if (offset < m_bitSize)
					{
						return offset;
					}
				}
			}
		}
		return -1;
	}

	int32_t firstContiguous(uint32_t size) const
	{
		for (uint32_t i = firstFree(); i < m_size; i++)
		{
			for (uint32_t j = 0; j < size; j++)
			{
				uint32_t offset = i + j;
				if (test(i + j))
				{
					break;
				}
				if (j + 1 == size)
				{
					return i;
				}
			}
		}
		return -1;
	}
};