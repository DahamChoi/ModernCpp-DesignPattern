#pragma once

#include <iostream>

class BankAccount
{
private:
	int balance = 0;
	int overdraft_limit = -500;

public:
	void deposit(int amount)
	{
		balance += amount;
		std::cout << "deposited " << amount << ", balance is now " << balance << std::endl;
	}

	bool withdraw(int amount)
	{
		if (balance - amount >= overdraft_limit)
		{
			balance -= amount;
			std::cout << "withdraw " << amount << ", balance is now " << balance << std::endl;
			return true;
		}
		return false;
	}
};