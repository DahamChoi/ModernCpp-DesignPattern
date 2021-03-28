#include "Painter.h"
#include <Windows.h>
#include <iostream>

void Painter::DrawPoints(std::vector<Point>::iterator start, std::vector<Point>::iterator end)
{
	for (auto i = start; i != end; i++)
	{
		gotoxy(i->x, i->y);
		std::cout << "бс";
	}
}

void Painter::gotoxy(int x, int y)
{
	COORD pos = { x,y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}
