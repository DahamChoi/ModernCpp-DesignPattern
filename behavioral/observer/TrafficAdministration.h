#pragma once

#include "Observer.h"
#include "Person.h"

#include <iostream>

class TrafficAdministration : public Observer<Person>
{
	void field_changed(Person& source, const std::string& field_name)
	{
		if (field_name == "age")
		{
			if (source.get_age() < 17)
				std::cout << "Whoa there, you are not old enough to drive!" << std::endl;
			else
			{
				std::cout << "We no longer care!" << std::endl;
				source.unsubscribe(this);
			}
		}
	}
};
