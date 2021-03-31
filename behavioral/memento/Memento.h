#pragma once

class Memento
{
private:
	int balance;

public:
	Memento(int balance) : balance{ balance } {}
	friend class BankAccount;
};