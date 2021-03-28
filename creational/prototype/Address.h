#pragma once

#include <string>

class Address
{
public:
	std::string street;
	std::string city;
	int suite;

public:
	Address(const Address& address);
	Address(const std::string& street, const std::string& city, int suite);
	Address& operator=(const Address& address);
};