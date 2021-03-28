#pragma once

#include <boost/container_hash/hash.hpp>

class Point
{
public:
	int x, y;

	friend size_t hash_value(const Point& obj)
	{
		size_t seed = 0x725C686F;
		boost::hash_combine(seed, obj.x);
		boost::hash_combine(seed, obj.y);
		return seed;
	}
};