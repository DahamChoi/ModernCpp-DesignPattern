#pragma once

#include "Shape.h"

class Circle : public Shape
{
private:
	float x, y, radius;

public:
	void draw() override
	{
		renderer.render_circle(x, y, radius);
	}

	void resize(float factor) override
	{
		radius *= factor;
	}

	Circle(Renderer& renderer, float x, float y, float radius)
		: Shape{ renderer }, x{ x }, y{ y }, radius{ radius } {}
};