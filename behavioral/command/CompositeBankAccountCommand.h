#pragma once

#include <vector>

#include "BankAccountCommand.h"

class CompositeBankAccountCommand 
	: public std::vector<BankAccountCommand>, public Command
{
public:
	CompositeBankAccountCommand(const std::initializer_list<value_type>& items)
		: vector<BankAccountCommand>(items) {}

	void call() override
	{
		bool ok = true;
		for (auto& cmd : *this)
		{
			if (ok)
			{
				cmd.call();
				ok = cmd.succeeded;
				if (!ok)
				{
					this->undo();
					this->succeeded = false;
				}
				else
				{
					this->succeeded = true;
				}
			}
			else
			{
				cmd.succeeded = false;
				this->succeeded = false;
			}
		}
	}

	void undo() override
	{
		for (auto it = rbegin(); it != rend(); ++it)
			it->undo();
	}
};