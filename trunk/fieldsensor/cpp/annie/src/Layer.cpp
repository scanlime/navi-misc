#include "../include/Layer.h"
#include "../include/Exception.h"

using namespace std;

namespace annie
{

Layer::Layer(int label)
{
	_size = 0;
	_label = label;
}

Layer::~Layer()
{
	vector<Neuron *>::iterator it;
	while (!_neurons.empty())
	{
		it = _neurons.begin();
		delete *it;
		it = _neurons.erase(it);
	}
}

VECTOR
Layer::getActivation()
{
	VECTOR answer;
	vector<Neuron *>::iterator it;
	for (it = _neurons.begin(); it!=_neurons.end(); it++)
	{
		Neuron *n = (Neuron *)(*it);
		answer.push_back(n->getActivation());
	}
	return answer;
}

VECTOR
Layer::getOutput()
{
	VECTOR answer;
	vector<Neuron *>::iterator it;
	for (it = _neurons.begin(); it!=_neurons.end(); it++)
	{
		Neuron *n = (Neuron *)(*it);
		answer.push_back(n->getOutput());
	}
	return answer;
}

Neuron &
Layer::getNeuron(int i)
{
	if (i<0 || i>=getSize())
	{
		string error(getClassName());
		error = error + "::getNeuron() - Invalid index specified";
		throw Exception(error);
	}
	return *(_neurons[i]);
}

int
Layer::getSize()
{	return _size;	}

int
Layer::getLabel()
{	return _label;	}

void
Layer::addNeuron(Neuron *nrn)
{
	_neurons.push_back(nrn);
	_size++;
}

const char *
Layer::getClassName()
{	return "Layer";	}

}; //namespace annie

