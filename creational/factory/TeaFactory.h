#pragma once

#include <memory>
#include "HotDrinkFactory.h"
#include "Tea.h"

class TeaFactory : public HotDrinkFactory
{
	std::unique_ptr<HotDrink> make() const override
	{
		return std::make_unique<Tea>();
	}
};