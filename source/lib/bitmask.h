#pragma once

#define DEFINE_BITMASK_OPERATORS(T)                                                    \
	inline constexpr T operator|(T a, T b)                                             \
	{                                                                                  \
		return static_cast<T>(static_cast<int>(a) | static_cast<int>(b));              \
	}                                                                                  \
	inline constexpr T operator&(T a, T b)                                             \
	{                                                                                  \
		return static_cast<T>(static_cast<int>(a) & static_cast<int>(b));              \
	}                                                                                  \
	inline constexpr T operator^(T a, T b)                                             \
	{                                                                                  \
		return static_cast<T>(static_cast<int>(a) ^ static_cast<int>(b));              \
	}                                                                                  \
	inline constexpr T operator~(T e) { return static_cast<T>(~static_cast<int>(e)); } \
	inline T		   operator|=(T a, T b)                                            \
	{                                                                                  \
		return a = static_cast<T>(static_cast<int>(a) | static_cast<int>(b));          \
	}                                                                                  \
	inline T operator&=(T a, T b)                                                      \
	{                                                                                  \
		return a = static_cast<T>(static_cast<int>(a) & static_cast<int>(b));          \
	}                                                                                  \
	inline T operator^=(T a, T b)                                                      \
	{                                                                                  \
		return a = static_cast<T>(static_cast<int>(a) ^ static_cast<int>(b));          \
	}

namespace std
{
	namespace Bitmask
	{
		constexpr bool test(uint8_t value, uint8_t mask) { return (value & mask) == mask; }
		constexpr void set(uint8_t value, uint8_t mask) { value |= mask; }
		constexpr void reset(uint8_t value, uint8_t mask) { value &= ~(mask); }
		constexpr bool isValid(uint8_t value) { return (value != 0) && (value & (value - 1)) == 0; }
	} // namespace Bitmask
} // namespace std