#include "TrafficAdministration.h"
#include "Person.h"

int main()
{
	TrafficAdministration tas;
	Person p{ 15 };
	p.subscribe(&tas);

	p.set_age(16);
	p.set_age(17);
}