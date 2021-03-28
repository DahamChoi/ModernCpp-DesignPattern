#pragma once

#include <map>
#include "Database.h"

class SingletonDatabase : public Database
{
private:
	SingletonDatabase()
	{
		// 데이터베이스에서 값을 읽어온다
	}

	std::map<std::string, int> capitals;

public:
	SingletonDatabase(SingletonDatabase const&) = delete;
	void operator=(SingletonDatabase const&) = delete;

	static SingletonDatabase& get()
	{
		static SingletonDatabase* db = new SingletonDatabase();
		return *db;
	}

	int get_population(const std::string& name) override
	{
		return capitals[name];
	}
};