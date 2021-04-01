#pragma once

#include "ListStrategy.h"

#include <sstream>

class HtmlListStrategy : public ListStrategy
{
public:
	void start(std::ostringstream& oss) override
	{
		oss << "<ul>" << std::endl;
	}

	void add_list_item(std::ostringstream& oss, const std::string& item) override
	{
		oss << "<li>" << item << "</li>" << std::endl;
	}
	
	void end(std::ostringstream& oss) override
	{
		oss << "</ul>" << std::endl;
	}
};