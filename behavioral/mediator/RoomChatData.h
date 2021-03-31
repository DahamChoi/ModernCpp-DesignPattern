#pragma once

#include "EventData.h"

#include <string>
#include <iostream>

class RoomChatData : public EventData
{
public:
	std::string origin;
	std::string message;

public:
	RoomChatData(const std::string& origin, const std::string& message)
		: origin{ origin }, message{ message } {}

	void print() const override
	{
		std::cout << origin << " : " << message << std::endl;
	}
};