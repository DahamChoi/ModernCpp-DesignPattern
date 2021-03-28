#pragma once

#include <string>
#include <vector>

#include "Database.h"

class ConfigurableRecordFinder
{
public:
	explicit ConfigurableRecordFinder(Database& db) : db{ db } {}

	int total_population(std::vector<std::string> names)
	{
		int result = 0;
		for (auto& name : names)
			result += db.get_population(name);
		return result;
	}

private:
	Database& db;
};