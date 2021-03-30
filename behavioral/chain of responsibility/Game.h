#pragma once

#include "Query.h"

#include <boost/signals2.hpp>

class Game
{
public:
	boost::signals2::signal<void(Query&)> queries;
};