#pragma once

#include <stdint.h>
#include <string.h>

template <typename T> class BitArray
{
	T*		 m_data = nullptr;
	size_t	 m_size = 0;
	uint32_t m_bitSize = 0;

public:
	BitArray() = default;
	BitArray(size_t size) : m_size(size)
	{
		memset(m_data, 0, size);
		m_bitSize = m_size * sizeof(T) * 8;
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
		uint8_t offset = pos == 0 ? 0 : pos / (sizeof(T) * 8);
		uint8_t bit = pos == 0 ? 0 : pos % (sizeof(T) * 8);
		m_data[offset] |= (1 << bit);
	}

	void reset(size_t pos)
	{
		uint8_t offset = pos == 0 ? 0 : pos / (sizeof(T) * 8);
		uint8_t bit = pos == 0 ? 0 : pos % (sizeof(T) * 8);
		m_data[offset] &= ~(1 << bit);
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

	// Tests a single index within the bitarray, returning true
	// if it is 1 and false if it is 0.
	bool test(size_t pos) const
	{
		uint8_t offset = pos == 0 ? 0 : pos / (sizeof(T) * 8);
		uint8_t bit = pos == 0 ? 0 : pos % (sizeof(T) * 8);
		return m_data[offset] & (1 << bit);
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
};
