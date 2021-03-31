#include <iostream>

#include "Person.h"

int main()
{
	ChatRoom cr;
	Person p1{ "jane", cr };
	Person p2{ "john", cr };

	p1.join();
	p2.join();

	p1.message("HI, JOHN");
	p2.message("HI, JANE");
}