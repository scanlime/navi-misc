#include "../include/InputLayer.h"
#include "../include/InputNeuron.h"
#include "../include/Exception.h"

using namespace std;

namespace annie
{

InputLayer::InputLayer(int label, int size) : Layer(label)
{
	_size = 0;
	for (int i=0;i<size;i++)
		addNeuron(new InputNeuron(label*Layer::MAX_LAYER_SIZE+i));
}

InputLayer::~InputLayer()
{
}

const char *
InputLayer::getClassName()
{	return "InputLayer";	}

void
InputLayer::setInput(real *input)
{
	vector<Neuron *>::iterator it;
	int i;
	InputNeuron *in;
	for (it = _neurons.begin(), i=0; it!=_neurons.end(); it++,i++)
	{
		in = (InputNeuron *)(*it);
		in->setValue(input[i]);
	}
}

void
InputLayer::setInput(VECTOR &input)
{
	if ((int)input.size() != getSize())
	{
		string error(getClassName());
		error = error + "::setInput() - Input vector provided has different size than size of layer";
		throw Exception(error);
	}
	vector<Neuron *>::iterator it;
	VECTOR::iterator it2;
	InputNeuron *in;
	for (it = _neurons.begin(), it2 = input.begin(); it!=_neurons.end(); it++, it2++)
	{
		in = (InputNeuron *)(*it);
		in->setValue(*it2);
	}
}

void
InputLayer::addNeuron(Neuron *nrn)
{
	if (strcmp(nrn->getClassName(),_INPUT_NEURON_STRING))
	{
		string error(getClassName());
		error = error + "::addNeuron() - Neuron added is not an InputNeuron";
		throw Exception(error);
	}
	Layer::addNeuron(nrn);
}

}; //namespace annie

