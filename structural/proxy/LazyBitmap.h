#pragma once

#include "Image.h"
#include "Bitmap.h"

#include <string>

class LazyBitmap : Image 
{
public:
	LazyBitmap(const std::string& filename) 
		: filename(filename) {}
	
	~LazyBitmap() 
	{
		delete bmp; 
	}
	
	void draw() override 
	{
		if (!bmp)
			bmp = new Bitmap(filename);
		bmp->draw();
	}

private:
	Bitmap* bmp{ nullptr };
	std::string filename;
};