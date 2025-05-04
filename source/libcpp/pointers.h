#pragma once

#include <stddef.h>
#include <stdint.h>

template <typename T>
class SharedPtr
{
	T*		m_ptr;
	size_t* m_count;

	void increment()
	{
		if (m_count)
		{
			(*m_count)++;
		}
	}

	void decrement()
	{
		if (m_count)
		{
			(*m_count)--;
		}
	}

	void release()
	{
		decrement();
		if (*m_count == 0)
		{
			delete m_ptr;
			delete m_count;
		}
	}

public:
	SharedPtr(T* ptr)
	{
		if (ptr)
		{
			m_ptr = ptr;
			m_count = new size_t(1);
		}
	}
	~SharedPtr() { release(); }
	SharedPtr(const SharedPtr<T>& other) : m_ptr(other.m_ptr), m_count(other.m_count)
	{
		if (m_count)
		{
			increment();
		}
	}

	size_t count() const { return m_count ? *m_count : 0; }

	SharedPtr<T>& operator=(const SharedPtr<T>& other)
	{
		if (this != &other)
		{
			release(); // Release current object
			m_ptr = other.m_ptr;
			m_count = other.m_count;
			if (m_count)
			{
				increment(); // Increment reference count
			}
		}

		return *this;
	}

	T* operator->() const { return m_ptr; }
	T& operator*() const { return *m_ptr; }
};

template <typename T, typename... Args>
SharedPtr<T> MakeShared(Args&&... args)
{
	return SharedPtr<T>(new T(std::forward<Args>(args)...));
}