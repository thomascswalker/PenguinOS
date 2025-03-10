#pragma once

#include <cstring.h>

void itoa(char* buffer, uint32_t value, uint32_t base);

namespace std
{
	EXTERN void* malloc(const uint32_t size);
	EXTERN void	 free(void* ptr);
	EXTERN void* realloc(void* ptr, const uint32_t size);

	template <typename T>
	struct RemoveReference
	{
		typedef T Type;
	};

	template <typename T>
	struct RemoveReference<T&>
	{
		typedef T Type;
	};

	template <typename T>
	struct RemoveReference<T&&>
	{
		typedef T Type;
	};

	template <typename T>
	constexpr typename RemoveReference<T>::Type&& move(T&& t) noexcept
	{
		return static_cast<typename RemoveReference<T>::Type&&>(t);
	}

	template <typename T>
	constexpr T&& forward(typename RemoveReference<T>::Type& t) noexcept
	{
		return static_cast<T&&>(t);
	}

	template <typename T>
	constexpr T&& forward(typename RemoveReference<T>::Type&& t) noexcept
	{
		return static_cast<T&&>(t);
	}

	template <typename T>
	void swap(T& t1, T& t2)
	{
		T tmp = std::move(t1);
		t1 = std::move(t2);
		t2 = std::move(tmp);
	}
} // namespace std