#pragma once

#include <string>
#include <vector>
#include <mutex>

#include "Observer.h"

template <typename T>
class Observable
{
public:
	void notify(T& source, const std::string& name)
	{
		std::vector<Observer<T>*> observer_copy;
		{
			std::scoped_lock<std::mutex> lock{ mtx };
			observer_copy = observers;
		}

		for (auto obs : observer_copy)
			obs->field_changed(source, name);
	}

	void subscribe(Observer<T>* f)
	{
		std::scoped_lock<std::mutex> lock{ mtx };
		observers.push_back(f);
	}

	void unsubscribe(Observer<T>* o)
	{
		std::scoped_lock<std::mutex> lock{ mtx };
		observers.erase(
			std::find(observers.begin(), observers.end(), o),
				observers.end());
	}

private:
	std::vector<Observer<T>*> observers;
	std::mutex mtx;
};