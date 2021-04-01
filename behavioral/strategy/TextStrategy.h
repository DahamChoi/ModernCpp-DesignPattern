#pragma once

#include "ListStrategy.h"

#include <vector>
#include <sstream>

template <typename LS>
class TextProcessor
{
public:
	void append_list(const std::vector<std::string> items)
	{
		list_strategy.start(oss);
		for (auto& item : items)
			list_strategy.add_list_item(oss, item);
		list_strategy.end(oss);
	}

	std::string str() const
	{
		return oss.str();
	}

private:
	std::ostringstream oss;
	LS list_strategy;
};