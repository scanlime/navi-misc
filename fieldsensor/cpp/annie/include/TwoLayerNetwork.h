#ifndef _TWOLAYERNETWORK_H
#define _TWOLAYERNETWORK_H

#include "MultiLayerNetwork.h"

namespace annie
{

/** Two layered networks are very commonly used. This is basically a
  * multi-layer perceptron network with only two layers - one hidden and one
  * output (the input is not counted as a layer).
  * This class basically derives from a MultiLayerNetwork and adds
  * functionality that make it easier to use if the network you're dealing
  * with has only 2 layers. There is nothing you can do with this class
  * that you can't do with MultiLayerNetwork, just that this may be easier to
  * use.
  */
class TwoLayerNetwork : public MultiLayerNetwork
{
public:
	/** Creates a two layer network
	  * @param inputs The number of inputs taken in by the network
	  * @param hidden The number of hidden neurons in the network
	  * @param outputs The size of the output vector (number of outputs given by the network)
	  */
	TwoLayerNetwork(int inputs, int hidden, int outputs);

	/** Loads a two-layer network from a file.
	  * The file is exactly the same as a MultiLayerNetwork file, and can
	  * be loaded there as well.
	  * @throws Exception if the file read does not correspond to a two layer network
	  */
	TwoLayerNetwork(const char *filename);

	/** Connects an input and a hidden neuron with a random weight
	 * @param input The index of the input neuron
	 * @param hidden The index of the hidden neuron in the hidden layer
	 */
	virtual void connect2in(int input, int hidden);

	/** Connects an input and a hidden neuron with the given weight
	 * @param input The index of the input neuron
	 * @param hidden The index of the hidden neuron in the hidden layer
	 * @param weight The weight of the connection
	 */
	virtual void connect2in(int input, int hidden, real weight);

	/** Connects a hidden and an output neuron with a random weight
	 * @param hidden The index of the hidden neuron in the hidden layer
	 * @param output The index of the output neuron in the output layer
	 */
	virtual void connect2out(int hidden, int output);
	
	/** Connects a hidden and an output neuron with the given weight
	  * @param hidden The index of the hidden neuron in the hidden layer
	  * @param output The index of the output neuron in the output layer
	  * @param weight The weight of the connection
	  */
	virtual void connect2out(int hidden, int output, real weight);

	/** Completely connects the network.
	  * All inputs are connected to all hidden neurons and all hidden neurons
	  * to all output neurons
	  */
	virtual void connectAll();

	/** Overrides MultiLayerNetwork::addLayer() so that it cannot be done.
	  * @throws Exception Because the number of layers of this class is fixed, so
	  *			you shouldn't be allowed to add a layer
	  */
	virtual void addLayer(int size);

	/// Returns "TwoLayerNetwork"
	virtual const char *getClassName();
};
}; //namespace annie
#endif // define _TWOLAYERNETWORK_H

