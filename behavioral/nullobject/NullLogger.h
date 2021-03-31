#pragma once

#include "Logger.h"

class NullLogger : public Logger
{
public:
	void info(const std::string s) {}
	void warn(const std::string s) {}
};