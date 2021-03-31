#pragma once

#include <boost/signals2/signal.hpp>

class ChatRoom
{
public:
	boost::signals2::signal<void(class EventData*)> events;
};