#include "EmployeeFactory.h"
#include "Address.h"

EmployeeFactory* EmployeeFactory::instance = nullptr;

EmployeeFactory::EmployeeFactory() 
	: main{ "", "London", "123 East Dr", 0 }, aux{ "", "Seoul", "124 Jongru", 0 }
{

}

EmployeeFactory::~EmployeeFactory()
{
	delete instance;
}

EmployeeFactory* EmployeeFactory::GetInstance()
{
	if (!instance) {
		instance = new EmployeeFactory();
	}

	return instance;
}

std::unique_ptr<Contact> EmployeeFactory::NewMainOfficeEmployee(std::string name, int suite)
{
	return NewEmployee(name, suite, main);
}

std::unique_ptr<Contact> EmployeeFactory::NewAuxOfficeEmployee(std::string name, int suite)
{
	return NewEmployee(name, suite, aux);
}

std::unique_ptr<Contact> EmployeeFactory::NewEmployee(
	std::string name, int suite, const Contact& proto)
{
	auto result = std::make_unique<Contact>(proto);
	result->name = name;
	result->address->suite = suite;
	return result;
}