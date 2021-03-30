#include <iostream>

#include "DoubleAttackModifier.h"

int main()
{
	Game game;
	Creature goblin{ game, "Boblin", 2, 2 };
	std::cout << goblin;
	{
		DoubleAttackModifier dam{ game, goblin };
		std::cout << goblin;
	}
	std::cout << goblin;
}