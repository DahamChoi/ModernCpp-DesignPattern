#pragma once

#include "Observable.h"

class Person : public Observable<Person>
{
private:
	int age;

public:
	Person(int age) : age{ age } {}

	int get_age() const { return age; }
	
	void set_age(int age)
	{
		if (this->age == age)	return;

		this->age = age;
		notify(*this, "age");
	}
};