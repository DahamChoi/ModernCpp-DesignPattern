#pragma once

#include "Renderer.h"

class Shape
{
protected:
	Renderer& renderer;
	Shape(Renderer& renderer) : renderer{ renderer } {}

public:
	virtual void draw() = 0;
	virtual void resize(float factor) = 0;
};