#include "../include/Exception.h"
#include "../include/RecurrentNeuron.h"

#include <cstdio>
using namespace std;

namespace annie
{

RecurrentNeuron::RecurrentNeuron(int label, bool hasBias) : SimpleNeuron(label,hasBias)
{
	_time=0;
	_lastOutput = _initialOutput = 0.0;
	_classHeirarchy.push_back(_RECURRENT_NEURON_STRING);
}

void
RecurrentNeuron::reset(real initActivation)
{
	_time = 0;
	_initialOutput = _lastOutput = _activationCache = initActivation;
	_outputCache = _activationFunction(_activationCache);
	_outputCacheValid = true;
}

void
RecurrentNeuron::step()
{
	invalidateOutputCache();
	_time++;
	LINKS::iterator it;
	for (it = _inputLinks.begin(); it != _inputLinks.end(); it++)
	{
		Link *l = (Link*)(*it);
		Neuron *from = l->getSource();
		if (from->instanceOf(_RECURRENT_NEURON_STRING))
		{
			RecurrentNeuron *nrn = (RecurrentNeuron*)from;
			while (nrn->getTime() != _time)
				nrn->step();
		}
	}
}

real
RecurrentNeuron::getNextOutput()
{
	step();
	return getOutput();
}

int
RecurrentNeuron::getTime()
{	return _time;	}

const char *
RecurrentNeuron::getClassName()
{	return _RECURRENT_NEURON_STRING; }

void 
RecurrentNeuron::calculateNewWeights(real learningRate)
{
	string s(getClassName());
	s = s + "::calculateNewWeights() - Method not yet implemented";
	throw Exception(s);
}
void 
RecurrentNeuron::update()
{
	string s(getClassName());
	s = s + "::update() - Method not yet implemented";
	throw Exception(s);
}

void 
RecurrentNeuron::setDesiredOutput(real desired)
{
	string s(getClassName());
	s = s + "::setDesiredOutput() - Method not yet implemented";
	throw Exception(s);
}

real
RecurrentNeuron::getOutput()
{
	if (_time == 0)
		return _initialOutput;
	return SimpleNeuron::getOutput();
}

real
RecurrentNeuron::getLastOutput()
{	return _lastOutput;	}

void
RecurrentNeuron::_recacheOutput()
{
	if (_outputCacheValid)
		return;
	LINKS::iterator it;

	_lastOutput = _outputCache;

	_activationCache = 0;
	for (it=_inputLinks.begin();it!=_inputLinks.end();it++)
	{
		Link *l = (Link *)(*it);
		Neuron *source = l->getSource();
		if (source->instanceOf(_RECURRENT_NEURON_STRING))
			_activationCache+=((RecurrentNeuron*)source)->getLastOutput()*l->getWeight();
		else
			_activationCache+=l->getSource()->getOutput()*l->getWeight();
	}
	if (_hasBias)
		_activationCache+=_bias;
	_outputCache = _activationFunction(_activationCache);
	_outputCacheValid = true;
}

}; //namespace annie

