#pragma once

class EventData
{
public:
	virtual ~EventData() = default;
	virtual void print() const = 0;
};