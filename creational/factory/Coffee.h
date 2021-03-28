#pragma once

#include <iostream>
#include "HotDrink.h"

class Coffee : public HotDrink
{
public:
	void prepare(int volume) override
	{
		std::cout << "boil water, pour " << volume << "ml, add espresso" << std::endl;
	}
};