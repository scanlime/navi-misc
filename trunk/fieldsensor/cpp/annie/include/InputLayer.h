#ifndef _INPUTLAYER_H
#define _INPUTLAYER_H

#include "Layer.h"
#include "InputNeuron.h"

namespace annie
{

/** A layer of input neurons */
class InputLayer : public Layer
{
public:
	/** Construct a layer with the given label and with the given number of 
	  * inputs.
	  * @param label The label to be given to this layer
	  * @param size The number of input in this layer
	  */
	InputLayer(int label, int size);
	virtual ~InputLayer();

	/// Sets the values of the input neurons in this layer to the values provided
	virtual void setInput(VECTOR &input);

	/// Sets the values of the input neurons in this layer to the values provided
	virtual void setInput(real *input);

	/** Adds a neuron to the input layer
	  * @param nrn An InputNeuron
	  * @throws Exception if the given neuron is not an InputNeuron
	  */
	virtual void addNeuron(Neuron *nrn);

	/// Returns "InputLayer"
	virtual const char *getClassName();
};
}; //namespace annie
#endif // define _INPUTLAYER_H

