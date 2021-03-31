#include <iostream>

#include "BankAccount.h"
#include "ConsoleLogger.h"

int main()
{
	BankAccount account{ "primary account", 1000, std::make_shared<ConsoleLogger>() };
	account.deposit(2000);
}