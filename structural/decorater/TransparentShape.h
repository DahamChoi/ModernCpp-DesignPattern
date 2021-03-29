#pragma once

#include "Shape.h"

#include <sstream>

template <typename T> class TransparentShape : public T
{
	static_assert(std::is_base_of<Shape, T>::value, "Template argument musb be a Shape");

public:
	uint8_t transparency;

public:
	template<typename...Args>
	TransparentShape(uint8_t transparency, Args ...args)
		: T(std::forward<Args>(args)...), transparency{ transparency } {}

	std::string str() const override
	{
		std::ostringstream oss;
		oss << static_cast<float>(transparency) / 255.f * 100.f
			<< "% transparency" << std::endl;
		return T::str() + oss.str();
	}
};