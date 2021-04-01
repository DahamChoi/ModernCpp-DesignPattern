#pragma once

#include "Game.h"

class Chess : public Game
{
public:
	explicit Chess() : Game{ 2 } {}

protected:
	void start() override {}
	bool have_winner() override { return turns == max_turns; }
	void take_turn() override
	{
		turns++;
		current_player = (current_player + 1) % number_of_players;
	}
	int get_winner() override { return current_player; }

private:
	int turns{ 0 }, max_turns{ 10 };
};