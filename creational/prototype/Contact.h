#pragma once

#include <string>
#include <iostream>

class Contact
{
public:
	std::string name;
	class Address* address = nullptr;
	
public:
	Contact(const Contact& contact);
	Contact(const std::string& name, const std::string& city, const std::string& street, int suite);
	Contact& operator=(const Contact& contact);
	~Contact();

	friend std::ostream& operator<<(std::ostream& os, const Contact& contact);
};