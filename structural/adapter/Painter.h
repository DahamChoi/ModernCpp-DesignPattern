#pragma once

#include <vector>

#include "Point.h"

class Painter
{
public:
	void DrawPoints(
		std::vector<Point>::iterator start, std::vector<Point>::iterator end);

private:
	void gotoxy(int x, int y);
};