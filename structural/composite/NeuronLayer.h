#pragma once

#include "Neuron.h"
#include "SomeNeurons.h"

class NeuronLayer : public std::vector<Neuron>, public SomeNeurons<NeuronLayer>
{
public:
	NeuronLayer(int count)
	{
		do 
		{
			emplace_back(Neuron{});
		} while (--count >= 0);
	}

	Neuron* begin() override
	{
		return (std::vector<Neuron>::begin()._Ptr);
	}

	Neuron* end() override
	{
		return (std::vector<Neuron>::end()._Ptr);
	}
};