#pragma once

#include <string>

class Address
{
public:
	std::string* house_name = nullptr;
};

class Person
{
public:
	Address* address = nullptr;
};