#ifndef _LINK_H
#define _LINK_H

#include "defines.h"

namespace annie
{

//forward declaration
class Neuron; 

/** Abstraction of a connection between two neurons.
  * A connection has a source, destination and a weight.
  * The source's output is taken as input by the destination
  */
class Link
{
private:
	/// The source neuron
	Neuron *_from;

	/// The destination neuron
	Neuron *_to;

	/// The weight of the connection
    real _weight;

	/** The change in weight to be made.
	  * Set this using setDeltaWeight() and then when you've set
	  * all weight changes then update the links using updateWeight()
	  * @see setDeltaWeight
	  * @see updateWeight
	  */
	real _deltaWeight;

	real _lastDeltaWeight;
public:
	/** Creates a link between two neurons.
	  * Weight assigned is a random value between -1 and +1
	  * @param to The destination neuron
	  * @param from The source neuron
	  */
	Link(Neuron *to, Neuron *from);

	/** Creates a link between two neurons.
	  * @param to The destination neuron
	  * @param from The source neuron
	  * @param weight The weight of the connection
	  */
	Link(Neuron *to, Neuron *from, real weight);

	/** Destroys the link.
	  * When doing so, removes the link from the _outputLinks of the
	  * source neuron and the _inputLinks of the destination neuron
	  */
	virtual ~Link();

	/// Pointer to the source neuron
	Neuron *getSource();

	/// Pointer to the destination neuron
	Neuron *getDestination();
	
	/// Returns the weight of the link
	real getWeight();

	/// Sets the weight of the link
	void setWeight(real weight);

	/** Sets the change in weight of the link
	  * The actual change is made only when updateWeight is called
	  * @see updateWeight
	  */
	void setDeltaWeight(real delta);

	/** Updates the weight of the link.
	  * New weight = old weight + value set by setDeltaWeight
	  * Then it resets the change in weight so successive calls
	  * to updateWeight have no adverse effect.
	  * @see setDeltaWeight
	  */
	void updateWeight();

	/** Finds if two links are equivalent.
	  * Two links are considered equivalent if they are between the 
	  * same two neurons, irrespective of the weight
	  * @return true if equivalent, false if not
	  */
	bool isEqualTo(Link *l);

	real getLastDeltaWeight();
};

}; //namespace annie
#endif // define _LINK_H

