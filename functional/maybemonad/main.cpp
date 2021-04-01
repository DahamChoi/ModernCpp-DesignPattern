#include <iostream>
#include "Data.h"
#include "Maybe.h"

void print_house_name(Person* p)
{
	if (p != nullptr && p->address != nullptr &&
		p->address->house_name != nullptr)
		std::cout << *p->address->house_name << std::endl;
}

void print_house_name_monad(Person* p)
{
	auto z = maybe(p)
		.With([](auto x) { return x->address; })
		.With([](auto x) { return x->house_name; })
		.Do([](auto x) { std::cout << *x << std::endl; });
}

int main()
{
	Person* p = new Person();
	p->address = new Address();
	p->address->house_name = new std::string("MY HOME!");
	print_house_name_monad(p);
}