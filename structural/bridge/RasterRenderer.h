#pragma once

#include <iostream>

#include "Renderer.h"

class RasterRenderer : public Renderer
{
public:
	void render_circle(float x, float y, float radius) override
	{
		std::cout << "Rasterizing circle of radius " << radius << std::endl;
	}
};