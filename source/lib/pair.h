#pragma once

template <typename Ta, typename Tb>
class Pair
{
public:
	Ta a;
	Tb b;

	Pair(const Ta& inA, const Tb& inB) : a(inA), b(inB) {}
};