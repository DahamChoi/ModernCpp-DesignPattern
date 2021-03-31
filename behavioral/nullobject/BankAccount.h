#pragma once

#include <memory>
#include <CastingInterop.h>

#include "OptionalLogger.h"

class BankAccount
{
private:
	std::shared_ptr<Logger> log;
	std::string name;
	int balance = 0;

public:
	BankAccount(const std::string& name, int balance, const std::shared_ptr<Logger>& logger = OptionalLogger::no_logging)
		: log{ std::make_shared<OptionalLogger>(logger) }, name{ name }, balance{ balance } {}

	void deposit(int amount)
	{
		balance += amount;
		log->info("Deposited $" + std::to_string(amount) + "to " + name +
			", balance is now $" + std::to_string(balance));
	}
};