#pragma once

#include "Shape.h"

#include <sstream>

class Square : public Shape
{
public:
	float length;

public:
	Square(float length) : length{ length } {}

	void resize(float factor)
	{
		length *= factor;
	}

	std::string str() const override
	{
		std::ostringstream oss;
		oss << "A square of length " << length << std::endl;
		return oss.str();
	}
};