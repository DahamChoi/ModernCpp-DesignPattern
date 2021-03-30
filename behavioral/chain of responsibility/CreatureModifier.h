#pragma once

#include "Creature.h"

class CreatureModifier
{
private:
	CreatureModifier* next{ nullptr };
	Game& game;
	Creature& creature;

public:
	explicit CreatureModifier(Game& game, Creature& creature)
		: game(game), creature(creature) {}

	void add(CreatureModifier* cm)
	{
		if (next) next->add(cm);
		else next = cm;
	}
};