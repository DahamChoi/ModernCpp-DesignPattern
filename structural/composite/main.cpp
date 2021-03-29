#include "Neuron.h"
#include "NeuronLayer.h"

int main()
{
	Neuron neuron, neuron2;
	NeuronLayer layer(2), layer2(2);

	neuron.connect_to(neuron2);
	neuron.connect_to(layer);
	layer.connect_to(neuron);
	layer.connect_to(layer2);
}