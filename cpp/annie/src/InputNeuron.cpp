#include "../include/InputNeuron.h"

using namespace std;
namespace annie
{

InputNeuron::InputNeuron(int label) : Neuron(label)
{
	_classHeirarchy.push_back(_INPUT_NEURON_STRING);
}

void
InputNeuron::_recacheOutput()
{	_outputCacheValid = true;	}

void
InputNeuron::_recacheError()
{	_errorCacheValid = true;	}

void
InputNeuron::setValue(real value)
{	
	_activationCache = _outputCache = value; 
	invalidateOutputCache();
	_recacheOutput();
}

string
InputNeuron::toString()
{
	string ans("Input");
	ans = ans + Neuron::toString();
	return ans;
}

const char *
InputNeuron::getClassName()
{
	return _INPUT_NEURON_STRING;
}

}; //namespace annie

