#ifndef _MULTILAYERNETWORK_H
#define _MULTILAYERNETWORK_H


#include "Layer.h"
#include "InputLayer.h"
#include "Network.h"
#include "TrainingSet.h"

namespace annie
{
/** Abstraction of a multi-layer perceptron network.
  * Basically, layers of SimpleNeurons constitute this network.
  * Training is done using the backpropagation technique which uses
  * the gradient descent method.
  *
  * The labels of the layers start from 0 (for the input layer) and then
  * keep moving on. The labels of neurons in the layers is =
  * <layer label>*Layer::MAX_LAYER_SIZE + <neuron index in layer>
  *
  * All neurons in the layers are allowed to have a bias.
  * \todo The copy constructor
  */
class MultiLayerNetwork : public Network
{
protected:
	/** Number of layers in the network.
	  * If you derive your own network from this class, the onus of
	  * keeping _nLayers consistent lies on you!
	  */
	int _nLayers;

	/// The layers
	std::vector<Layer *> _layers;

	/// The input layer
	InputLayer *_inputLayer();

	/// The output layer
	Layer *_outputLayer();
public:
	/** Create a multi-layer network.
	  * @param inputs The number of inputs taken in by the network
	  */
	MultiLayerNetwork(int inputs);

	/// Copy constructor, NOT YET IMPLEMENTED
	MultiLayerNetwork(MultiLayerNetwork &srcNet);

	/** Loads a multi-layer network from the given filename.
	  * @param filename The name of the file from which to load the network
	  * @throws Exception On any error
	  */
	MultiLayerNetwork(const char *filename);

	virtual ~MultiLayerNetwork();

	/// Adds a layer of the given size to the network. This new layer becomes the output layer
	virtual void addLayer(int size);

	/** Completely connects the given layer with the layer below it, i.e., all
	  * neurons in the given layer will give their output to all the neurons in the
	  * layer below. Weights are random.
	  * @param srcLayer The index of the source layer
	  * @throws Exception If the layer index provided is invalid
	  */
	virtual void connectLayer(int srcLayer);

	/** Connects two neurons, with random weight
	  * @param srcLayer The index of the layer in which the source neuron lies
	  * @param srcNrn The index of the source neuron in the source layer
	  * @param destNrn The index of the destination neuron (will be in the layer with index srcLayer+1)
	  * @throws Exception On any invalid argument values
	  */
	virtual void connect(int srcLayer, int srcNrn, int destNrn);

	/** Connects two neurons, with the weight provided.
	  * @param srcLayer The index of the layer in which the source neuron lies
	  * @param srcNrn The index of the source neuron in the source layer
	  * @param destNrn The index of the destination neuron (will be in the layer with index srcLayer+1)
	  * @param weight The weight of the link
	  * @throws Exception On any invalid argument values
	  */
	virtual void connect(int srcLayer, int srcNrn, int destNrn, real weight);

	/** Sets the bias of the given neuron.
	  * @param layer The index of the layer in which the neuron lies
	  * @param nrn The index of the neuron in the layer
	  * @param bias The bias to be given to the neuron
	  */
	virtual void setBias(int layer, int nrn, real bias);

	/// The number of layers in the network (does not count the input layer as a layer)
	virtual int getLayerCount();

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

	/** Trains the network with data from the given TrainingSet using the
	  * backpropagation algorithm.
	  * @param T The TrainingSet containing input/desired-output vector pairs
	  * @param epochs The number of epochs to train the network. An epoch is a single
	  *				iteration through all input/desired-output vector pairs in T.
	  * @param learningRate The learning rate to be used for weight updation
	  * @param momentum The momentum factor to be used during weight updation. 0 by default.
	  */
	virtual void train(TrainingSet &T, int epochs, real learningRate, real momentum = 0.0);
	
	/** Trains the network with data from the given TrainingSet using the
	  * backpropagation algorithm.
	  * @param T The TrainingSet containing input/desired-output vector pairs
	  * @param epochs The number of epochs to train the network. An epoch is a single
	  *				iteration through all input/desired-output vector pairs in T.
	  * @param learningRate The learning rate to be used for weight updation
	  * @param momentum The momentum factor to be used during weight updation
	  * @param verbose If true, prints the current epoch number onto stdout, if false
	  *				no such status information is given.
	  */
	virtual void train(TrainingSet &T, int epochs, real learningRate, real momentum, bool verbose);

	/** Saves the network to the given filename.
	  * The file is a simple text file, open it up in a text editor
	  * to see the format, quite simple
	  * @param filename The name of the file to save the network in.
	  */
	virtual void save(const char *filename);

	/** Sets the activation function used by the neurons in the provided layer.
	  * @param layer The layer whose activation function is to be changed.
	  *				 layer>0 (as input neurons don't have any activation function)
	  *				 and layer<getLayerCount()
	  * @param f The activation function to be used
	  * @param df The derivative of the activation function to be used. Required for training.
	  * @throws Exception if an invalid layer is given
	  */
	virtual void setActivationFunction(int layer, ActivationFunction f, ActivationFunction df);

	/// Returns "MultiLayerNetwork"
	virtual const char *getClassName();
};
}; //namespace annie
#endif // define _MULTILAYERNETWORK_H

