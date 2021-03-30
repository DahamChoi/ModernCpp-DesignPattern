#pragma once

class Command
{
public:
	virtual void call() = 0;
	virtual void undo() = 0;

	bool succeeded;
};