#pragma once

#include "BankAccount.h"
#include "Command.h"

class BankAccountCommand : public Command
{
public:
	enum Action { deposit, withdraw };

private:
	BankAccount& account;
	Action action;
	int amount;
	bool withdrawl_succeeded;

public:
	BankAccountCommand(BankAccount& account, const Action action, const int amount)
		: account(account), action(action), amount(amount), withdrawl_succeeded{ false } {}

	void call() override
	{
		switch (action)
		{
		case deposit:
			account.deposit(amount);
			succeeded = true;
			break;
		case withdraw:
			succeeded = withdrawl_succeeded = account.withdraw(amount);
			break;
		}
	}

	void undo() override
	{
		switch (action)
		{
		case withdraw:
			if (withdrawl_succeeded)
			{
				account.deposit(amount);
				succeeded = true;
			}
			else
			{
				succeeded = false;
			}
			break;
		case deposit:
			account.withdraw(amount);
			succeeded = true;
			break;
		}
	}
};