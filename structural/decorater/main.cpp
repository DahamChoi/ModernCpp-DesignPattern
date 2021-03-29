#include "ColoredShape.h"
#include "TransparentShape.h"
#include "Circle.h"
#include "Square.h"

#include <iostream>

int main()
{
	ColoredShape<TransparentShape<Square>> square = { "red", 51, 5 };

	std::cout << square.str();
}