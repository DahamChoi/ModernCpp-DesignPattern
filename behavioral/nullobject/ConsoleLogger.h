#pragma once

#include "Logger.h"

#include <iostream>

class ConsoleLogger : public Logger
{
public:
	void info(const std::string& s) override
	{
		std::cout << "INFO : " << s << std::endl;
	}

	void warn(const std::string& s) override
	{
		std::cout << "WARRNING!!! " << s << std::endl;
	}
};