#pragma once

#include <string>
#include <ostream>

#include "Game.h"
#include "Query.h"

class Creature
{
private:
	Game& game;
	int attack, defense;

public:
	std::string name;

	Creature(Game& game, const std::string& name, int attack, int defense)
		: game{ game }, name { name }, attack{ attack }, defense{ defense } {}

	friend std::ostream& operator<<(std::ostream& os, Creature& creature)
	{
		os << "name : " << creature.name << std::endl;
		os << "attack : " << creature.get_attack() << std::endl;
		os << "defense : " << creature.defense << std::endl;
		return os;
	}

	int get_attack() const
	{
		Query q{ name, Query::Argument::attack, attack };
		game.queries(q);
		return q.result;
	}
};