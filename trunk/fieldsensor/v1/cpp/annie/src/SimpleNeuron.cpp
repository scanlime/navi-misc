#include "../include/Exception.h"
#include "../include/SimpleNeuron.h"

#include <cstdio>
using namespace std;

namespace annie
{

SimpleNeuron::SimpleNeuron(int label, bool hasBias) : Neuron(label)
{
	_activationFunction = sigmoid;
	_dActivationFunction = dsigmoid;
	_hasBias = hasBias;
	_bias = _deltaBias = 0.0;
	_classHeirarchy.push_back(_SIMPLE_NEURON_STRING);
}

void
SimpleNeuron::connect(Neuron *from, real weight)
{
	Link *link = new Link(this,from,weight);
	disconnect(from);
	_inputLinks.push_back(link);
	from->_outputLinks.push_back(link);
	invalidateOutputCache();
}

void
SimpleNeuron::connect(Neuron *from)
{
	connect(from,random());
}

void
SimpleNeuron::setActivationFunction(ActivationFunction f, ActivationFunction df)
{
	_activationFunction = f;
	_dActivationFunction = df;
	invalidateOutputCache();
	invalidateErrorCache();
}

void
SimpleNeuron::setDesiredOutput(real desired)
{
	if (getOutputCount()!=0)
	{
		string error(getClassName());
		error = error + "::setDesiredOutput() - Called for a non-output neuron";
		throw Exception(error);
	}
	//real output = getOutput();
	//real PI = 3.1415926535897932384626433832795;
	//if (output > 2*PI)
	//	output -= 2*PI;
	//real temp = (desired - output);
	//if (temp > PI)
	//	temp = 2*PI - temp;
	invalidateErrorCache();
	_errorCache = _dActivationFunction(getActivation())*(desired-getOutput());
	_recacheError();
}

void
SimpleNeuron::_recacheError()
{
	if (getOutputCount()==0) //ie an output neuron
		_errorCacheValid = true;
	if (_errorCacheValid)
		return;
	//Note that this work will be done only if this is a NON-OUTPUT neuron
	//ie, it's output is taken as input by other neurons
	LINKS::iterator it;
	_errorCache = 0;
	for (it = _outputLinks.begin(); it!=_outputLinks.end(); it++)
	{
		Link *l = (Link *)(*it);
		_errorCache+=(l->getWeight()*l->getDestination()->getError());
	}
	_errorCache = _errorCache * _dActivationFunction(getActivation());
	_errorCacheValid = true;
}

void
SimpleNeuron::_recacheOutput()
{
	if (_outputCacheValid)
		return;
	LINKS::iterator it;

	_activationCache = 0;
	for (it=_inputLinks.begin();it!=_inputLinks.end();it++)
	{
		Link *l = (Link *)(*it);
		_activationCache+=l->getSource()->getOutput()*l->getWeight();
	}
	if (_hasBias)
		_activationCache+=_bias;
	_outputCache = _activationFunction(_activationCache);
	_outputCacheValid = true;
}
	
void
SimpleNeuron::calculateNewWeights(real learningRate, real momentum)
{
	if (getInputCount()==0)
	{
		string error(getClassName());
		error = error + "::calculateNewWeights() - Called for a neuron with no inputs";
		throw Exception(error);
	}
	LINKS::iterator it;
	for (it=_inputLinks.begin();it!=_inputLinks.end();it++)
	{
		Link *link = (Link *)(*it);
		real input = link->getSource()->getOutput();
		link->setDeltaWeight(getError()*input*learningRate+momentum*link->getLastDeltaWeight());
	}
	if (_hasBias)
		_deltaBias = getError()*learningRate;
}

void
SimpleNeuron::update()
{
	LINKS::iterator it;
	for (it=_inputLinks.begin();it!=_inputLinks.end();it++)
	{
		Link *link = (Link*)(*it);
		link->updateWeight();
	}
	if (_hasBias)
	{
		_bias+=_deltaBias;
		_deltaBias=0.0;
	}
	invalidateOutputCache();
	invalidateErrorCache();
}

void
SimpleNeuron::removeBias()
{
	_hasBias = false;
	_bias = 0.0;
	invalidateOutputCache();
	invalidateErrorCache();
}

void
SimpleNeuron::setBias(real bias)
{
	if (!_hasBias)
		throw Exception("SimpleNeuron::setBias() - This neuron isn't supposed to have a bias");
	_bias = bias;
	invalidateOutputCache();
}

real
SimpleNeuron::getBias()
{	return _bias;	}

bool
SimpleNeuron::hasBias()
{	return _hasBias;	}

string
SimpleNeuron::toString()
{
	string ans = Neuron::toString();
	if (_hasBias)
	{
		char buffer[1000];
		sprintf(buffer,"- Bias = %f\n",getBias());
		ans = ans + buffer;
	}
	return ans;
}

const char *
SimpleNeuron::getClassName()
{	
	return _SIMPLE_NEURON_STRING;
}

real
SimpleNeuron::getWeight(Neuron *from)
{
	LINKS::iterator i;
	for (i=_inputLinks.begin(); i!=_inputLinks.end(); i++)
	{
		Link *l = (Link*)(*i);
		if (l->getSource() == from)
			return l->getWeight();
	}
	return 0.0;
}
} //namespace annie


