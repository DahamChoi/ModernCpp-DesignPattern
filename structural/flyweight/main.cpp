#include "FormattedText.h"

#include <iostream>

int main()
{
	FormattedText ft("This is a brave new world");
	ft.get_range(10, 15).capitalize = true;
	std::cout << ft << std::endl;
}