#pragma once

#include <memory>
#include "HotDrinkFactory.h"
#include "Coffee.h"

class CoffeeFactory : public HotDrinkFactory
{
	std::unique_ptr<HotDrink> make() const override
	{
		return std::make_unique<Coffee>();
	}
};