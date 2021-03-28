#pragma once

#include <iostream>
#include "HotDrink.h"

class Tea : public HotDrink
{
public:
	void prepare(int volume) override
	{
		std::cout << "Take tea bag, boil water, pour " << volume << "ml, add some lemon" << std::endl;
	}
};