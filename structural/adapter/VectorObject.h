#pragma once

#include <vector>

#include "Line.h"

class VectorObject
{
public:
	virtual std::vector<Line>::iterator begin() = 0;
	virtual std::vector<Line>::iterator end() = 0;
};