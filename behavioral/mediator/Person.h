#pragma once

#include <string>
#include <vector>

#include "ChatRoom.h"
#include "RoomChatData.h"

class Person
{
private:
	std::string name;
	std::vector<std::string> chat_log;
	ChatRoom& chatRoom;

public:
	Person(const std::string& name, ChatRoom& chatRoom)
		: name{ name }, chatRoom{ chatRoom } 
	{
		chatRoom.events.connect([&](EventData* e)
			{
				RoomChatData* rcd = dynamic_cast<RoomChatData*>(e);
				if (e)
				{
					if(rcd->origin.compare(Person::name) != 0)
						e->print();
				}
			});
	}

	void join()
	{
		RoomChatData rcd{ name, "join" };
		chatRoom.events(&rcd);
	}

	void message(const std::string& message)
	{
		RoomChatData rcd{ name, message };
		chatRoom.events(&rcd);
	}
};