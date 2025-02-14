#pragma once

#include <cstring.h>

void		 itoa(char* buffer, uint32_t value, uint32_t base);
EXTERN void* malloc(const uint32_t size);
EXTERN void	 free(const void* ptr);

namespace std
{
	// Minimal implementation of remove_reference
	template <typename T>
	struct removeReference
	{
		typedef T type;
	};

	template <typename T>
	struct removeReference<T&>
	{
		typedef T type;
	};

	template <typename T>
	struct removeReference<T&&>
	{
		typedef T type;
	};

	// Implementation of std::move
	template <typename T>
	constexpr typename removeReference<T>::type&& move(T&& t) noexcept
	{
		return static_cast<typename removeReference<T>::type&&>(t);
	}

	// Implementation of std::forward
	template <typename T>
	constexpr T&& forward(typename removeReference<T>::type& t) noexcept
	{
		return static_cast<T&&>(t);
	}

	template <typename T>
	constexpr T&& forward(typename removeReference<T>::type&& t) noexcept
	{
		return static_cast<T&&>(t);
	}
} // namespace std