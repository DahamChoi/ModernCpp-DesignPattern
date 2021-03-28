#include "Address.h"

Address::Address(const Address& address) 
	: street{ address.street }, city{ address.city }, suite{ address.suite } 
{

}

Address::Address(const std::string& street, const std::string& city, int suite)
	: street{ street }, city{ city }, suite{ suite }
{

}

Address& Address::operator=(const Address& address)
{
	if (this == &address)
		return *this;

	street = address.street;
	city = address.city;
	suite = address.suite;
	return *this;
}


