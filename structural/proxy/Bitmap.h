#pragma once

#include "Image.h"

#include <string>
#include <iostream>

class Bitmap : public Image
{
public:
	Bitmap(const std::string& filename)
	{
		std::cout << "Loading image from " << filename << std::endl;
	}

	void draw() override
	{
		std::cout << "Drawing image" << std::endl;
	}
};