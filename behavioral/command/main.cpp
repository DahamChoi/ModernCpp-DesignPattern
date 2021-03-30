#include <iostream>

#include "BankAccount.h"
#include "BankAccountCommand.h"

int main()
{
	BankAccount ba;
	BankAccountCommand cmd{ ba, BankAccountCommand::Action::deposit, 100 };
	cmd.call();
}