#pragma once

#include <iostream>

#include "Neuron.h"

template <class Self>
class SomeNeurons
{
public:
	template <class T> void connect_to(T& other)
	{
		for (Neuron& from : *static_cast<Self*>(this))
		{
			for (Neuron& to : other)
			{
				from.out.push_back(&to);
				to.in.push_back(&from);

				std::cout << "MY ID IS : " << from.id << std::endl;
				std::cout << "OUT : " << to.id << std::endl;
			}
		}
	}

	virtual class Neuron* begin() = 0;
	virtual class Neuron* end() = 0;
};