#include "Contact.h"
#include "Address.h"
#include <iostream>

Contact::Contact(const Contact& contact) 
	: name{ contact.name }, address{ new Address(*contact.address) } 
{

}

Contact::Contact(const std::string& name, const std::string& city, const std::string& street, int suite)
{
	this->name = name;
	address = new Address(city, street, suite);
}

Contact& Contact::operator=(const Contact& contact)
{
	if (this == &contact)
		return *this;
	name = contact.name;
	address = new Address(*contact.address);
	return *this;
}

Contact::~Contact()
{
	delete address;
}

std::ostream& operator<<(std::ostream& os, const Contact& contact)
{
	os << "---------------------------------------" << std::endl;
	os << "name : " << contact.name << std::endl;
	os << "city : " << contact.address->city << std::endl;
	os << "street : " << contact.address->street << std::endl;
	os << "suite : " << contact.address->suite << std::endl;
	os << "---------------------------------------" << std::endl;

	return os;
}
