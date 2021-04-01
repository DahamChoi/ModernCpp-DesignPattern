#pragma once

#include "ListStrategy.h"

#include <iostream>
#include <sstream>

class MarkdownListStrategy : public ListStrategy
{
public:
	void add_list_item(std::ostringstream& oss, const std::string& item) override
	{
		oss << " * " << item << std::endl;
	}
};