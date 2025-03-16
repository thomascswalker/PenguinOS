#pragma once

#include <cstring.h>

void itoa(char* buffer, uint32_t value, uint32_t base);

namespace std
{
	EXTERN void* malloc(const uint32_t size);
	EXTERN void	 free(void* ptr);
	EXTERN void* realloc(void* ptr, const uint32_t size);

	template <typename ValueType>
	struct RemoveReference
	{
		typedef ValueType Type;
	};

	template <typename ValueType>
	struct RemoveReference<ValueType&>
	{
		typedef ValueType Type;
	};

	template <typename ValueType>
	struct RemoveReference<ValueType&&>
	{
		typedef ValueType Type;
	};

	template <typename ValueType>
	constexpr typename RemoveReference<ValueType>::Type&& move(ValueType&& t) noexcept
	{
		return static_cast<typename RemoveReference<ValueType>::Type&&>(t);
	}

	template <typename ValueType>
	constexpr ValueType&& forward(typename RemoveReference<ValueType>::Type& t) noexcept
	{
		return static_cast<ValueType&&>(t);
	}

	template <typename ValueType>
	constexpr ValueType&& forward(typename RemoveReference<ValueType>::Type&& t) noexcept
	{
		return static_cast<ValueType&&>(t);
	}

	template <typename ValueType>
	void swap(ValueType& t1, ValueType& t2)
	{
		ValueType tmp = std::move(t1);
		t1 = std::move(t2);
		t2 = std::move(tmp);
	}
} // namespace std