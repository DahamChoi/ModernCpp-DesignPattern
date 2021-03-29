#pragma once

#include "Shape.h"

#include <sstream>

class Circle : public Shape
{
public:
	float radius;

public:
	Circle(float radius) : radius{ radius } {}

	void resize(float factor)
	{
		radius *= factor;
	}

	std::string str() const override
	{
		std::ostringstream oss;
		oss << "A circle of radius " << radius << std::endl;
		return oss.str();
	}
};