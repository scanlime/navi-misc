#ifndef _CENTERNEURON_H
#define _CENTERNEURON_H

#include "Neuron.h"

namespace annie
{

/** A center-neuron, the building block of a Radial basis function network.
  * Neurons of this type have a "center" which is a D-dimensional point in space,
  * where D = the number of inputs taken by the neuron.
  * The activation of this neuron is the euclidean distance between the input 
  * vector and the center point.
  * The output of this neuron is typically the result of the gaussian distribution
  * function applied to the activation
  * \todo Implement gradient-descent rule based updation of center-point
  */
class CenterNeuron : public Neuron
{
protected:
	/// The center point
	real *_center;

	/// The dimension of the center point = the size of the input vector
	int _dimension;

	/// @see Neuron::_recacheOutput
	virtual void _recacheOutput();

	/// @see Neuron::_recacheError
	virtual void _recacheError();
	
	/** Derivative of the activation function. 
	  * Used for gradient descent rule based updation of weights and
	  * centers. Not yet implemented
	  */
	ActivationFunction _dActivationFunction;

public:
	/** Constructs a center with the given label and with the 
	  * center point a random point in the given dimensional space.
	  * @param label The label to be given to the neuron
	  * @param dimension The dimension of the center-point = size of input vector
	  */
	CenterNeuron(int label, int dimension);

	/** Constructs a center with the given label and given center
	  * @param label The label to be given to the neuron
	  * @param center The center point
	  */
	CenterNeuron(int label, VECTOR center);

	/** Constructs a center with the given label and given center
	  * @param label The label to be given to the neuron
	  * @param dimension The dimension of the center-point = size of input vector
	  * @param center The center point
	  */
	CenterNeuron(int label, int dimension, real center[]);
	virtual ~CenterNeuron();

	/// Returns the center point
	VECTOR getCenter();

	/// Sets the center point
	virtual void setCenter(VECTOR center);

	/// Sets the center point
	virtual void setCenter(real center[]);
	
	/// Sets the center-neuron to receive as input the output of the given neuron
	virtual void connect(Neuron *from);
	
	/// @see Neuron::toString
	virtual std::string toString();

	/// Returns "CenterNeuron"
	virtual const char *getClassName();
	
	/** Sets the activation function of the neuron and its derivative
	  * @param f The activation function to be used (gaussian by default)
	  * @param df The derivative of the activation function (dgaussian by default).
	  *				Required for gradient descent training which is NOT YET IMPLEMENTED
	  */
	virtual void setActivationFunction(ActivationFunction f, ActivationFunction df);

	/// The dimension of the center
	virtual int getDimension();
};

}; //namespace annie
#endif // define _CENTERNEURON_H

