#pragma once

#include "Database.h"

#include <map>

class DummyDatabase : public Database
{
private:
	std::map<std::string, int> capitals;

public:
	DummyDatabase()
	{
		capitals["seoul"] = 9648606;
		capitals["newyork"] = 8491079;
		capitals["tokyo"] = 13952915;
	}

	int get_population(const std::string& name) override 
	{
		return capitals[name];
	}
};