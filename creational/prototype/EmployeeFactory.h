#pragma once

#include <memory>

#include "Contact.h"

class EmployeeFactory
{
private:
	static class EmployeeFactory* instance;

	const Contact main;
	const Contact aux;

	EmployeeFactory();
	~EmployeeFactory();

public:
	static class EmployeeFactory* GetInstance();

	std::unique_ptr<Contact> NewMainOfficeEmployee(std::string name, int suite);
	std::unique_ptr<Contact> NewAuxOfficeEmployee(std::string name, int suite);

private:
	std::unique_ptr<Contact> NewEmployee(std::string name, int suite, const Contact& proto);
};