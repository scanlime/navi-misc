#ifndef _RADIALBASISNETWORK_H
#define _RADIALBASISNETWORK_H

#include "Network.h"
#include "SimpleNeuron.h"
#include "CenterNeuron.h"
#include "InputLayer.h"
#include "TrainingSet.h"

namespace annie
{

/** A Radial Basis Function Neural Network.
  *
  * The network consists of a layer of N-inputs, then h-(N-dimensional) centers, and 
  * some outputs.
  * The default activation function for the centers is gaussian(), which is the gaussian
  * distribution function with sigma = 1. If you want to change that then you'll have
  * to write your own activation function and change it with setCenterActivationFunction().
  * The output neurons use the identity() function as the activation function, thus the 
  * output is simply the weighted sum of the outputs of the centers.
  *
  * There are atleast two ways to train a radial basis network.
  *
  * One involves fixed centers and training the weights only in order to minimize
  * the error over a training set. The other involves using the gradient descent
  * rule to adjust both centers and weights. Currently only the former is implemented
  * in annie using trainWeights().
  *
  * @see trainWeights
  * \todo Implement gradient-descent rule based updation of centers and weights
  * \todo The copy constructor
  */
class RadialBasisNetwork : public Network
{
protected:
	/** Number of centers in the network.
	  * If you plan to extend this class, then the onus of keeping this value
	  * consistent lies on you
	  */
	int _nCenters;

	/// Layer of input. Each member is an InputNeuron
	InputLayer *_inputLayer;

	/// Layer of centers, each member is a CenterNEuron
	Layer *_centerLayer;

	/// Layer of output, each member if a SimpleNeuron
	Layer *_outputLayer;

public:
	/** Creates a Radial basis function network. All the outputs will have a bias.
	  * @param inputs Number of inputs taken in by the network
	  * @param centers Number of centers the network has. Each center will be 
	  *			an inputs-dimensional point
	  * @param outputs The number of outputs given by the neuron. All of them will have
	  *			a bias
	  */
	RadialBasisNetwork(int inputs, int centers, int outputs);

	/** Loads a network from a text file
	  * @see save
	  * @param filename Name of the file from which to load network structure
	  * @throws Exception On any error
	  */
	RadialBasisNetwork(const char *filename);

	/// Copy constructor, NOT YET IMPLEMENTED
	RadialBasisNetwork(RadialBasisNetwork &srcNet);

	virtual ~RadialBasisNetwork();

	/** Sets the ith center point to the given point.
	  * @param i The center that is to be changed
	  * @param center The getInputCount() dimensional point
	  */
	virtual void setCenter(int i, VECTOR &center);

	/** Sets the ith center point to the given point.
	  * @param i The center that is to be changed
	  * @param center The getInputCount() dimensional point
	  */
	virtual void setCenter(int i, real *center);

	/** Returns the point corresponding to the ith center.
	  * @param i The center whose point is wanted
	  * @return The getInputCount() dimensional point corresponding to the 
	  *		ith center
	  */
	virtual VECTOR getCenter(int i);

	/// The number of centers in the network
	virtual int getCenterCount();

	/** Sets the bias of the ith output.
	  * @param i The index of the output (0<=i<getOutputCount()).
	  * @param bias The bias to be given to that output.
	  * @throws Exception if that neuron isn't allowed to be biased.
	  */
	virtual void setBias(int i, real bias);

	/** Returns the bias of the ith output
	  * @param i The index of the output (0<=i<getOutputCount()).
	  * @return The bias of the output. If there is no bias, it returns 0.0
	  */
	virtual real getBias(int i);

	/** Prevents the ith output from having any bias.
	  * @param i The index of the output (0<=i<getOutputCount()).
	  * @throws Exception if the index given is invalid
	  */
	virtual void removeBias(int i);

    /** Sets the weight between the given center and output
	  * @param center Index of the center (0<=center<getCenterCount()).
	  * @param output Index of the output (0<=output<getOutputCount()).
	  * @param weight The weight to give to the link between the center and output.
	  * @throws Exception if any of the parameters given is invalid
	  */
	virtual void setWeight(int center, int output, real weight);

    /** Returns the weight of the link between the given center and output
	  * @param center Index of the center (0<=center<getCenterCount()).
	  * @param output Index of the output (0<=output<getOutputCount()).
	  * @throws Exception if any of the parameters given is invalid
	  */
	virtual real getWeight(int center, int output);

	/** Returns the output of the network for the given input.
	  * @param input A vector of getDimension() reals
	  * @return The corresponding output of the network
	  */
	virtual VECTOR getOutput(VECTOR &input);

	/** Wrapper function to allow getOutput() to work for an array
	  * of real as input as well.
	  * Does exactly the same thing as Network::getOutput(real*).
	  */
	virtual VECTOR getOutput(real *input);

	/** Sets the activation function of the center neurons.
	  * (The activation function is gaussian by default)
	  * @param f The activation function to be used.
	  * @param df The derivation of the activation function, used in gradient descent training
	  */
	void setCenterActivationFunction(ActivationFunction f, ActivationFunction df);
	
	/** Trains the weights of the network, centers are kept fixed.
	  * @param T The TrainingSet from which input/desired-output pairs will be obtained
	  */
	void trainWeights(TrainingSet &T);
	//void trainCentersAndWeights(TrainingSet &T, int epochs, real learningRate);

	///Returns "RadialBasisNetwork"
	virtual const char *getClassName();

	///Save the network to a text file
	virtual void save(const char *filename);
};
}; //namespace annie
#endif // define _RADIALBASISNETWORK_H

