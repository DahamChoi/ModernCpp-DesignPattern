#pragma once

#include <vector>
#include <memory>

#include "Memento.h"

using namespace std;

class BankAccount
{
	int balance = 0;
	vector<shared_ptr<Memento>> changes;
	int current;

public:
	explicit BankAccount(const int balance) : balance(balance)
	{
		changes.emplace_back(make_shared<Memento>(balance));
		current = 0;
	}

	friend ostream& operator<<(ostream& os, BankAccount& obj)
	{
		os << "balance : " << obj.balance;
		return os;
	}

	shared_ptr<Memento> depiosit(int amount)
	{
		balance += amount;
		auto m = make_shared<Memento>(balance);
		changes.push_back(m);
		++current;
		return m;
	}

	void restore(const shared_ptr<Memento>& m)
	{
		if (m)
		{
			balance = m->balance;
			changes.push_back(m);
			current = changes.size() - 1;
		}
	}

	shared_ptr<Memento> undo()
	{
		if (current > 0)
		{
			--current;
			auto m = changes[current];
			balance = m->balance;
			return m;
		}
		return {};
	}

	shared_ptr<Memento> redo()
	{
		if (current + 1 < changes.size())
		{
			++current;
			auto m = changes[current];
			balance = m->balance;
			return m;
		}
		return {};
	}
};