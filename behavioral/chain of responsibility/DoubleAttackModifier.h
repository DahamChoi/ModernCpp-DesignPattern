#pragma once

#include "CreatureModifier.h"

#include <boost/signals2/connection.hpp>

class DoubleAttackModifier : public CreatureModifier
{
private:
	boost::signals2::connection conn;

public:
	DoubleAttackModifier(Game& game, Creature& creature)
		: CreatureModifier(game, creature)
	{
		conn = game.queries.connect([&](Query& q)
		{
				if (q.creature_name == creature.name &&
					q.argument == Query::Argument::attack)
					q.result *= 2;
		});
	}

	~DoubleAttackModifier()
	{
		conn.disconnect();
	}
};