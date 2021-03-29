#pragma once

#include <iostream>

#include "Renderer.h"

class VectorRenderer : public Renderer
{
public:
	void render_circle(float x, float y, float radius) override
	{
		std::cout << "Drawing a vector circle of radius " << radius << std::endl;
	}
};