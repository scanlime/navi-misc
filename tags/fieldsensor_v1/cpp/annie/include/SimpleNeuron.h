#ifndef _SIMPLENEURON_H
#define _SIMPLENEURON_H

#include "Neuron.h"
#include "defines.h"

namespace annie
{

/** A simple perceptron - i.e., it takes as input the weighted sum of the
  * outputs of the neurons connected to it.
  * Probably the first type of neuron you would come across in any 
  * introductory literature of neural networks.
  */
class SimpleNeuron : public Neuron
{
protected:
	/// Is this neuron allowed to have a bias?
	bool _hasBias;

	/// If allowed to have a bias then the bias, otherwise 0.0
	real _bias;

	/// The change in bias, calculated using the gradient descent rule
	real _deltaBias;

	/// The derivative of the activation function, required for gradient descent training
	ActivationFunction _dActivationFunction;

	/// @see Neuron::_recacheOutput
	virtual void _recacheOutput();

	/// @see Neuron::_recacheError
	virtual void _recacheError();

public:
	/** Creates a simple neuron with the given label.
	  * @param label The label to be given to the neuron
	  * @param hasBias true if the neuron is allowed to have a bias, false otherwise. Default is true
	  * @see removeBias
	  */
	SimpleNeuron(int label, bool hasBias = true);

	/** Sets the bias of the neuron.
	  * @param bias The bias to be given to the neuron
	  * @throws Exception if the neuron is not allowed to have a bias
	  */
	virtual void setBias(real bias);
	
	/// Is the neuron allowed to have a bias?
	virtual bool hasBias();
	
	/// The bias of the neuron, 0.0 if it's not allowed to have a bias
	virtual real getBias();

	/// Sets bias to 0.0 and prevents the neuron from having a bias
	virtual void removeBias();

	/** Sets the desired output of the neuron.
	  * Should be called only for output neurons, i.e., those whose output is not
	  * connected to anyone else. Setting the desired output at these neurons
	  * will form the basis of error backpropagation
	  * @param desired The desired output of this neuron
	  * @throws Exception if the neuron is not an output neuron
	  */
	virtual void setDesiredOutput(real desired);

	/** Sets the activation function and its derivative (required for error backpropagation)
	  * @param f The activation function to be used
	  * @param df The derivative of the activation function
	  */
	virtual void setActivationFunction(ActivationFunction f, ActivationFunction df);

	/** Connects the given neuron to this one, i.e., the output of the supplied neuron
	  * will be given as input to this neuron. A random weight is provided to the link
	  * @param from The neuron whose output is to be taken is as input
	  */
	virtual void connect(Neuron *from);

	/** Connects the given neuron to this one, i.e., the output of the supplied neuron
	  * will be given as input to this neuron. The weight of the link will be the one
	  * supplied.
	  * @param from The neuron whose output is to be taken as input
	  * @param weight The weight of the connection
	  */
	virtual void connect(Neuron *from, real weight);
	
	/** Calculates the adjustment to incoming weights based on the gradient
	  * descent rule (backpropagation).
	  * Doesn't actually update the weights, just sets the value.
	  * @see update
	  */
	virtual void calculateNewWeights(real learningRate, real momentum=0.0);

	/** Updates the weights of incoming connections according to the values
	  * calculated using the gradient descent rule.
	  * @see calculateNewWeights
	  */
	virtual void update();

	/// @see Neuron::toString
	virtual std::string toString();

	/// Returns "SimpleNeuron"
	virtual const char *getClassName();

	/** Returns the weight of the link between the provided neuron and this neuron.
	  * @param from The neuron whose output is connected to this neuron's input
	  * @return The weight of the connection, 0.0 if no connection exists
	  */
	virtual real getWeight(Neuron *from);
};

}; //namespace annie
#endif // define _SIMPLENEURON_H

