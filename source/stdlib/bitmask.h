
#define DEFINE_BITMASK_OPERATORS(T)                                           \
	inline constexpr T operator|(T a, T b)                                    \
	{                                                                         \
		return static_cast<T>(static_cast<int>(a) | static_cast<int>(b));     \
	}                                                                         \
	inline constexpr T operator&(T a, T b)                                    \
	{                                                                         \
		return static_cast<T>(static_cast<int>(a) & static_cast<int>(b));     \
	}                                                                         \
	inline constexpr T operator^(T a, T b)                                    \
	{                                                                         \
		return static_cast<T>(static_cast<int>(a) ^ static_cast<int>(b));     \
	}                                                                         \
	inline constexpr T operator~(T e)                                         \
	{                                                                         \
		return static_cast<T>(~static_cast<int>(e));                          \
	}                                                                         \
	inline T operator|=(T a, T b)                                             \
	{                                                                         \
		return a = static_cast<T>(static_cast<int>(a) | static_cast<int>(b)); \
	}                                                                         \
	inline T operator&=(T a, T b)                                             \
	{                                                                         \
		return a = static_cast<T>(static_cast<int>(a) & static_cast<int>(b)); \
	}                                                                         \
	inline T operator^=(T a, T b)                                             \
	{                                                                         \
		return a = static_cast<T>(static_cast<int>(a) ^ static_cast<int>(b)); \
	}
