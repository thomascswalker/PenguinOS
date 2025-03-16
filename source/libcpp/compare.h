namespace std
{
	template <typename ValueType>
	struct Less
	{
		constexpr bool operator()(const ValueType& lhs, const ValueType& rhs) const
		{
			return lhs < rhs;
		}
	};
	template <typename ValueType>
	struct Greater
	{
		constexpr bool operator()(const ValueType& lhs, const ValueType& rhs) const
		{
			return lhs > rhs;
		}
	};
} // namespace std