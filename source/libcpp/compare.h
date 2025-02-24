namespace std
{
	template <typename T>
	struct Less
	{
		constexpr bool operator()(const T& lhs, const T& rhs) const { return lhs < rhs; }
	};
	template <typename T>
	struct Greater
	{
		constexpr bool operator()(const T& lhs, const T& rhs) const { return lhs > rhs; }
	};
} // namespace std