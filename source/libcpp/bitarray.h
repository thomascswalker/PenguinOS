#pragma once

#include <stdio.h>

template <typename T>
class BitArray
{
	using ValueType = T;
	using PointerType = T*;
	using SizeType = size_t;
	PointerType m_data = nullptr;
	SizeType	m_size = 0;
	SizeType	m_bitSize = 0;

public:
	// Size is equal to the size in bytes this map will
	// take up.
	BitArray(SizeType size = 0)
	{
		m_size = size;
		m_bitSize = m_size * sizeof(T) * 8;
		memset(m_data, 0, m_size * sizeof(T));
	}

	PointerType data() const { return m_data; }

	SizeType size() const { return m_size; }

	SizeType bitSize() const { return m_bitSize; }

	void set(SizeType pos)
	{
		SizeType index = pos == 0 ? 0 : pos / (sizeof(T) * 8);
		uint8_t	 offset = pos == 0 ? 0 : pos % (sizeof(T) * 8);
		m_data[index] |= (1 << offset);
	}

	void reset(SizeType pos)
	{
		SizeType index = pos == 0 ? 0 : pos / (sizeof(T) * 8);
		uint8_t	 offset = pos == 0 ? 0 : pos % (sizeof(T) * 8);
		m_data[index] &= ~(1 << offset);
	}

	void fill(bool state)
	{
		T value = state ? 0xFFFFFFFF : 0x0;
		memset(m_data, value, m_size);
	}

	void fill(bool state, SizeType pos, SizeType size)
	{
		T value = state ? 0xFFFFFFFF : 0x0;
		memset(m_data + pos, value, size / sizeof(T));
	}

	void clear() { memset(m_data, 0, m_size); }

	// Tests a single index within the bitarray, returning true
	// if it is 1 and false if it is 0.
	bool test(SizeType pos) const
	{
		SizeType index = pos == 0 ? 0 : pos / (sizeof(T) * 8);
		uint8_t	 offset = pos == 0 ? 0 : pos % (sizeof(T) * 8);
		return (m_data[index] & (1 << offset)) != 0;
	}

	bool all() const
	{
		for (SizeType i = 0; i < m_size; i++)
		{
			if (!test(i))
			{
				return false;
			}
		}
		return true;
	}

	bool all(SizeType pos, SizeType size)
	{
		for (SizeType i = pos; i < pos + size; i++)
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
		for (SizeType i = 0; i < m_size; i++)
		{
			if (test(i))
			{
				return true;
			}
		}
		return false;
	}

	bool any(SizeType pos, SizeType size) const
	{
		for (SizeType i = pos; i < pos + size; i++)
		{
			if (test(i))
			{
				return true;
			}
		}
		return false;
	}

	bool empty() const { return !any(); }

	int32_t firstFree() const
	{
		for (SizeType i = 0; i < m_size; i++)
		{
			if (m_data[i] == 0xFFFFFFFF)
			{
				continue;
			}
			for (SizeType j = 0; j < 32; j++)
			{
				SizeType offset = i * 32 + j;
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

	int32_t firstContiguous(SizeType size) const
	{
		for (SizeType i = firstFree(); i < m_size; i++)
		{
			for (SizeType j = 0; j < size; j++)
			{
				SizeType offset = i + j;
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
