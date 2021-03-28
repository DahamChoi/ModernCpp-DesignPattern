#pragma once

#include <string>

class Database
{
public:
	virtual int get_population(const std::string& name) = 0;
};