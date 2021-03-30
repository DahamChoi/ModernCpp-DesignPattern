#pragma once

#include "CompositeBankAccountCommand.h"

class MoneyTransferCommand : public CompositeBankAccountCommand
{
public:
	MoneyTransferCommand(BankAccount& from, BankAccount& to, int amount)
		: CompositeBankAccountCommand
	{
		BankAccountCommand{
			from, BankAccountCommand::withdraw, amount}, 
		BankAccountCommand{
			to, BankAccountCommand::deposit, amount }
	} {}
};