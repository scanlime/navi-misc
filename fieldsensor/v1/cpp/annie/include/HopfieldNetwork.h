#ifndef _HOPFIELDNETWORK_H
#define _HOPFIELDNETWORK_H

#include "Network.h"
#include "RecurrentNeuron.h"
#include "Matrix.h"
#include <vector>

namespace annie
{

/** Similar to the signum function, but used for non-bipolar Hopfield networks.
  * @param x A real value
  * @return 0 if x<=0, 1 otherwise.
  */
real isPositive(real x);

/** A Hopfield network - content addressable memory and energy.
  * The Hopfield network is considered a landmark in the field of neural computing.
  * This model can store a number of patterns and retrieve the correct pattern starting
  * from a noisy or incomplete pattern so that a network can be trained to identify
  * patterns or images. Hopfield proposed this model as a model of biological memory
  * (often called "content addressable memory"), similar to the way a human brain
  * recognizes an image based on the past experience or learning.
  *
  * Hopfield also introduced another important idea by establishing a new connection
  * between computation and physics with the concept of energy in his model and showed
  * that computation can be viewed as a transition to lower energy states.
  */
class HopfieldNetwork : public Network
{
protected:
	/// true if patterns are bipolar (ie, elements are -1 or 1), false if not (elements are 0 or 1)
	bool _bipolar;

	/// The number of patterns that have been given to the network to be stored
	int _nPatterns;

	/// The weight matrix of the network
	Matrix *_weightMatrix;

	/// The recurrent neurons, each of them is connected to everyone else with weights from the weight matrix
	RecurrentNeuron** _neurons;

	virtual bool _equal(std::vector<int> &p1, std::vector<int> &p2);
	int _time;

public:
	/** Creates a Hopfield network, which can store bipolar binary patterns
	  * (i.e., patterns consisting of -1's and 1's). 
	  * Diagonal elments of the weight matrix will be zero.
	  * Neurons will NOT have a bias.
	  * @param size The size/number of bits of/in the patterns to be stored
	  */
	HopfieldNetwork(int size);

	/** Creates a Hopfield network, which can store binary patterns.
	  * Diagonal elments of the weight matrix will be zero.
	  * @param size The size/number of bits of/in the patterns to be stored
	  * @param bias true if the neurons are to have a bias, false otherwise
	  * @param bipolar true if patterns are bipolar (elements are -1 or 1), false if not
	  *					(elements are 0 or 1).
	  */
	HopfieldNetwork(int size, bool bias, bool bipolar);

	/** Loads a Hopfield network from a file.
	  * @see save
	  * @param filename The name of the filename to load the network from
	  * @throws Exception on any error
	  */
	HopfieldNetwork(const char *filename);

	/// Cleans up memory by getting rid of the weight matrix and neurons
	virtual ~HopfieldNetwork();

	/** Adds a pattern to the network by making suitable adjustments to the weight
	  * matrix.
	  * @param pattern The bipolar pattern (of getSize() length) to be stored
	  * @throws Exception If the given pattern is not bipolar (i.e., consisting of only -1s and 1s) and
	  *			the network is designed for bipolar patters or if the pattern does not consist of 0s and 1s
	  */
	virtual void addPattern(int pattern[]);

	/// Returns the energy at the current state of the network.
	virtual real getEnergy();

	/** Returns the energy of the supplied pattern.
	  * Energy of a pattern depends on the weight matrix
	  */
	virtual real getEnergy(int pattern[]);

	/// Returns the size of the patterns that can be stored (i.e., length of the pattern vector)
	virtual int getSize();

	/// Steps the network in time
	virtual void step();

	/** The number of steps the network has taken since the last call to getOutput
	  * @see getNextOutput
	  * @see getOutput
	  */
	virtual int getTime();

	/** The number of patterns that the network has been instructed to store
	  * This will be the same as the number of calls to addPattern
	  * @see addPattern
	  */
	virtual int getPatternCount();

	/// Returns a copy of the weight matrix that is used by this network
	virtual Matrix getWeightMatrix();

	/// Returns "HopfieldNetwork"
	virtual const char* getClassName();

	/** Saves the network to a file
	  * \todo Implement this!
	  * @param filename The name of the file to store the network into so that it
	  *					can be loaded from there later
	  */
	virtual void save(const char *filename);

	/** Sets the weight between the ith and jth neuron.
	  * Since the weights are symmetruc, the weight between
	  * neuron i and neuron j will be the same as the weight
	  * between neuron j and neuron i, which will be the same
	  * as the weight supplied.
	  * @param i The ith neuron
	  * @param j The jth neuron
	  * @param weight The weight of the link (i,j) and (j,i)
	  */
	virtual void setWeight(int i, int j, real weight);

	/* Given an input pattern, keeps iterating through time till the network
	  * output converges. Ofcourse, it is possible that this never happens
	  * and hence a timeout has to be specified.
	  * \todo Implement this!
	  * @param pattern The initial input pattern given to the network
	  * @param updateAll Determines type of updating (synchronous, asynchronous)
	  * @param timeout The maximum number of iteration to try convergence for
      * @return false if the network output didn't converge till the timeout, true otherwise
	  */
	//virtual bool converge(int pattern[], bool updateAll, int timeout);

	/** Get the bias at the given neuron
	  * @param i The neuron whose bias is wanted
	  * @return The bias
	  */
	virtual real getBias(int i);

	/** Sets the bias of the given neuron.
	  * @param i The neuron whose bias is to be set
	  * @param bias The bias value to give the ith neuron
	  */
	virtual void setBias(int i, real bias);

	/** Sets the initial output of the neurons to the values
	  * in the given array and resets time to 0.
	  * @param pattern An array containing the initial outputs of the 
	  *                neurons. The array must contain getSize() elements
	  */
	virtual void setInput(int pattern[]);
	
	/** Sets the initial output of the neuron to the values
	  * in the given vector of integers and sets time to 0.
	  * @param pattern A vector of integers containing the initial outputs
	  *                of the neurons. The vector must contain getSize()
	  *                elements, each -1 or 1 (if bipolar) or 0 or 1 if not.
	  */
	virtual void setInput(std::vector<int> &pattern);
	
	/** Returns the current outputs of the neurons.
	  * @return A vector v, where v[i] = current output at the ith neuron
	  */
	virtual std::vector<int> getOutput();
	
	/** Steps the network through one unit of time and returns
	  * the output.
	  * @return A vector v, where v[i] = output of the ith neuron
	  */
	virtual std::vector<int> getNextOutput();
	
	/** The HopfieldNetwork generally deals with vectors of integers
	  * only because the neurons are binary.
	  * But since this class implements the Network class, it must
	  * provide this function as well, which returns the same thing
	  * as setInput() following by getOutput() would, except that the
	  * neuron's output will be of type real instead of int
	  */
	virtual VECTOR getOutput(VECTOR &input);
	
	/** Sets the initial input of the network and then steps through
	  * time, returning true if the network converges before the given
	  * timeout.
	  * @param pattern The initial output of the network
	  * @param timeout The maximum number of steps the network should go through
	  * @return true if the pattern converges within the timeout, false otherwise
	  */
	virtual bool propagate(int pattern[], int timeout);
	
	/** Sets the initial input of the network and then steps through
	  * time, returning true if the network converges before the given
	  * timeout.
	  * @param pattern The initial output of the network
	  * @param timeout The maximum number of steps the network should go through
	  * @return true if the pattern converges within the timeout, false otherwise
	  */
	virtual bool propagate(std::vector<int> &pattern, int timeout);
};
}; //namespace annie
#endif // define _HOPFIELDNETWORK_H

