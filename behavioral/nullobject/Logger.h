#pragma once

#include <string>

class Logger
{
public:
	virtual ~Logger() = default;
	virtual void info(const std::string& s) = 0;
	virtual void warn(const std::string& s) = 0;
};