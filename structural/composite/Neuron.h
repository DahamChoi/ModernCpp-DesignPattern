#pragma once

#include <vector>

#include "SomeNeurons.h"

class Neuron : public SomeNeurons<Neuron>
{
private:
	std::vector<Neuron*> in, out;
	unsigned int id;

	friend class SomeNeurons;

public:
	Neuron()
	{
		static int id = 1;
		this->id = id++;
	}

	Neuron* begin() override
	{
		return this;
	}

	Neuron* end() override
	{
		return this + 1;
	}
};