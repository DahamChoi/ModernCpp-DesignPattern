#pragma once

#include <string>

class Query
{
public:
	std::string creature_name;
	enum Argument{ attack, defense } argument;
	int result;
};