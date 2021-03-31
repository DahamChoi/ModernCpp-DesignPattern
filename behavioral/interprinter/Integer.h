#pragma once

#include "Element.h"

class Integer : public Element
{
public:
	int value;

	explicit Integer(const int value) : value{ value } {}

	int eval() const override { return value; }
};