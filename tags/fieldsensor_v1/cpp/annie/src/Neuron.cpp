#include "../include/Exception.h"
#include "../include/Neuron.h"
#include "../include/Link.h"

#include <cmath>
#include <cstdio>
using namespace std;

namespace annie
{

real identity(real x)
{	return x;	}

real didentity(real x)
{	return 1.0;	}

real sigmoid(real x)
{	return (real)(1/(1+exp(-x)));	}

real dsigmoid(real x)
{
	real f=sigmoid(x);
	return f*(1-f);
}

real gaussian(real x)
{
	return (real)(exp(-1*x*x));
}

real dgaussian(real x)
{
	return -2*gaussian(x)*x;
}

real signum(real x)
{
	if (x<0.0)
		return (real)-1.0;
	return (real)1.0;
}

real tansig(real x) 
{ return (real)((2/(1+exp(-2*x)))-1); } 

real dtansig(real x) 
{ 
	real f=tansig(x); 
	return 1-(f*f); 
} 

real random()
{
	return (real)((rand()-RAND_MAX/2.0)/RAND_MAX);
}

real random2()
{
	real r = (unsigned int)rand();
	r = r/RAND_MAX;
	return r;
}

int randomInt(int low, int high)
{
	return rand()%(high-low)+low;
}

Neuron::Neuron(int label)
{
	_label = label;
	_inputLinks.clear();
	_outputLinks.clear();
	_errorCache = _activationCache = _outputCache = 0.0;
	_outputCacheValid = _errorCacheValid = true;
	_activationFunction = identity;
	_classHeirarchy.push_back("Neuron");
}

Neuron::Neuron(Neuron &neuron)
{
	//string error(getClassName());
	string error("Neuron");
	error = error + "::Neuron() - Copy constructor not implemented";
	throw Exception(error);
}

void
Neuron::invalidateOutputCache()
{
	if (_outputCacheValid)
	{
		_outputCacheValid = false;
		LINKS::iterator it;
		for (it=_outputLinks.begin();it!=_outputLinks.end();it++)
		{
			Link *l = (Link *)(*it);
			l->getDestination()->invalidateOutputCache();
		}
	}
}

void
Neuron::invalidateErrorCache()
{
	if (_errorCacheValid)
	{
		_errorCacheValid = false;
		LINKS::iterator it;
		for (it =  _inputLinks.begin(); it != _inputLinks.end(); it++)
		{
			Link *l = (Link *)(*it);
			l->getSource()->invalidateErrorCache();
		}
	}
}

real
Neuron::getActivation()
{
	_recacheOutput();
	return _activationCache;
}

real
Neuron::getOutput()
{
	_recacheOutput();
	return _outputCache;
}

real
Neuron::getError()
{
	_recacheError();
	return _errorCache;
}

int
Neuron::getInputCount()
{	return _inputLinks.size(); }

int
Neuron::getOutputCount()
{	return _outputLinks.size(); }

int
Neuron::getLabel()
{	return _label;	}

Neuron::~Neuron()
{
	LINKS::iterator it;
	while (!_inputLinks.empty())
	{
		it = _inputLinks.begin();
		Link *l = (Link *)(*it);
		delete l;
	}
	while (!_outputLinks.empty())
	{
		it = _outputLinks.begin();
		Link *l = (Link *)(*it);
		delete l;
	}
}

string
Neuron::toString()
{
	string ans;
	char buffer[1000];
	
	sprintf(buffer,"%s(%d)\n",getClassName(),getLabel());
	ans=ans+buffer;
	
	sprintf(buffer,"- Inputs  : %d\n",getInputCount());
	ans=ans+buffer;
	if (getInputCount() > 0)
	{
		sprintf(buffer,"  (source,weight)      = ");
		ans = ans + buffer;
		LINKS::iterator it;
		for (it=_inputLinks.begin(); it!=_inputLinks.end(); it++)
		{
			Link *l = (Link *)(*it);
			sprintf(buffer,"(%d,%f) ",l->getSource()->getLabel(),l->getWeight());
			ans=ans+buffer;
		}
		ans=ans+"\n";
	}
		
	sprintf(buffer,"- Outputs : %d\n",getOutputCount());
	ans=ans+buffer;
	if (getOutputCount() > 0)
	{
		sprintf(buffer,"  (destination,weight) = ");
		ans = ans + buffer;
		LINKS::iterator it;
		for (it=_outputLinks.begin(); it!=_outputLinks.end(); it++)
		{
			Link *l = (Link *)(*it);
			sprintf(buffer,"(%d,%f) ",l->getDestination()->getLabel(),l->getWeight());
			ans=ans+buffer;
		}
		ans=ans+"\n";
	}
	return ans;
}

void
Neuron::disconnect(Neuron *from)
{
	Link *link = new Link(this,from,0.0);
	LINKS::iterator it;	
	for (it=_inputLinks.begin();it!=_inputLinks.end();it++)
	{
		Link *l = (Link *)(*it);
		if (l->isEqualTo(link))
		{
			delete l;
			break;
		}
	}
	invalidateOutputCache();
	delete link;
}

int
Neuron::getInputs(vector<int> &labels, VECTOR &weights)
{
	labels.clear();
	weights.clear();
	LINKS::iterator it;
	for (it = _inputLinks.begin(); it!=_inputLinks.end(); it++)
	{
		Link *l = (Link *)(*it);
		labels.push_back(l->getSource()->getLabel());
		weights.push_back(l->getWeight());
	}
	return getInputCount();
}

real
Neuron::getWeight(Neuron *from)
{
	LINKS::iterator it;
	Link *l;
	for (it = _inputLinks.begin(); it!=_inputLinks.end(); it++)
	{
		l = (Link*)(*it);
		if (l->getSource() == from)
			return l->getWeight();
	}
	return 0.0;
}

ostream& operator << (ostream& s, Neuron &d)
{
	s<<d.toString();
	return s;
}

bool
Neuron::instanceOf(const char *className)
{
	vector<char *>::iterator it;
	for (it = _classHeirarchy.begin(); it!= _classHeirarchy.end(); it++)
	{
		char *name = (char*)(*it);
		if (!strcmp(name,className))
			return true;
	}
	return false;
}

} //namespace annie
