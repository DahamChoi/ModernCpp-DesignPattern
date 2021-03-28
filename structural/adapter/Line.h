#pragma once

#include "Point.h"

class Line
{
public:
	Point start, end;

	friend size_t hash_value(const Line& obj)
	{
		size_t seed = 0x719E6B16;
		boost::hash_combine(seed, obj.start);
		boost::hash_combine(seed, obj.end);
		return seed;
	}
};