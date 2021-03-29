#pragma once

#include "Shape.h"

#include <sstream>

template <typename T> class ColoredShape : public T
{
	static_assert(std::is_base_of<Shape, T>::value, "Template argument musb be a Shape");

public:
	std::string color;

public:
	template<typename...Args>
	ColoredShape(const std::string& color, Args ...args)
		: T(std::forward<Args>(args)...), color{ color } {}

	std::string str() const override
	{
		std::ostringstream oss;
		oss << "has the color " << color << std::endl;
		return T::str() + oss.str();
	}
};