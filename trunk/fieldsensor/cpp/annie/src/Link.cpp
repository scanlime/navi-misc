#include "../include/Link.h"
#include "../include/Neuron.h"

using namespace std;

namespace annie
{

Link::Link(Neuron *to, Neuron *from)
{
	_from = from;
	_to = to;
	_deltaWeight = 0;
	_lastDeltaWeight = 0;
	setWeight(random());
}

Link::Link(Neuron *to, Neuron *from, real weight)
{	
	_from = from;
	_to = to;
	_deltaWeight = 0;
	_lastDeltaWeight = 0;
	setWeight(weight);	
}

void
Link::setWeight(real weight)
{	
	_weight = weight;	
	_to->invalidateOutputCache();
	_from->invalidateErrorCache();
}

bool
Link::isEqualTo(Link *l)
{
	return (l->_from == this->_from && l->_to == this->_to);
}

Link::~Link()
{
	LINKS::iterator it;
	
	for (it=_to->_inputLinks.begin();it!=_to->_inputLinks.end();it++)
	{
		Link *l=(Link *)(*it);
		if (isEqualTo(l))
		{
			it = _to->_inputLinks.erase(it);
			break;
		}
	}

	for (it=_from->_outputLinks.begin();it!=_from->_outputLinks.end();it++)
	{
		Link *l=(Link *)(*it);
		if (isEqualTo(l))
		{
			it = _from->_outputLinks.erase(it);
			break;
		}
	}
}

Neuron *
Link::getSource()
{	return _from; }

Neuron *
Link::getDestination()
{	return _to; }

real
Link::getWeight()
{	return _weight;	}

void
Link::setDeltaWeight(real delta)
{	
	_lastDeltaWeight = _deltaWeight;
	_deltaWeight = delta; 
}

real
Link::getLastDeltaWeight()
{
	return _deltaWeight;
}

void
Link::updateWeight()
{
	_weight += _deltaWeight;
	_deltaWeight = 0.0;
}

}; //namespace annie

