#ifndef _LAYER_H
#define _LAYER_H

#include "Neuron.h"
#include "Link.h"

namespace annie
{

/** Abstraction for a "layer" of neurons, i.e., a group of neurons not
  * connected to each other.
  * @see InputLayer
  */
class Layer
{
protected:
	/// The label of the layer
	int _label;

	/** The number of neurons in the layer.
	  * If you create a sub-class of this class, then the onus
	  * of ensuring that this value is consistent lies on you!
	  */
	int _size;

	/// The neurons in this layer.
	std::vector<Neuron *> _neurons;

public:
	/** The maximum number of neurons in a layer
	  * Needed for some automatic label assignments of neurons and layers
	  * in Networks
	  */
	static const int MAX_LAYER_SIZE = MAX_NEURONS_IN_LAYER;

	/** Constructs a layer with the given label */
	Layer(int label);

	virtual ~Layer();

	/// Returns the label of the layer
	virtual int getLabel();

	/// The size of the layer (number of neurons in it)
	virtual int getSize();

	/// Adds the given neuron to the layer
	virtual void addNeuron(Neuron *nrn);

	/** Gives the ith reference in the layer.
	  * @param i The index of the neuron in the layer (0<=i<getSize()).
	  * @throws Exception if the index given is invalid
	  */
	virtual Neuron& getNeuron(int i);
	
	/// The activation vector formed by the activations of individual neurons in the layer
	virtual VECTOR getActivation();
	
	/// The output vector formed by the outputs of individual neurons in the layer
	virtual VECTOR getOutput();

	/// Returns "Layer"
	virtual const char *getClassName();
};
}; //namespace annie
#endif // define _LAYER_H

