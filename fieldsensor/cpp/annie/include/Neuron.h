#ifndef _NEURON_H
#define _NEURON_H

#include "Link.h"
#include "defines.h"
#include <string>

namespace annie
{

// Some activation functions and their derivatives

/// The identity function, ie, f(x) = x
real identity(real x);
/// Derivative of the identity function, thus always returns 1
real didentity(real x);
/// Sigmoidal activation f(x)
real sigmoid(real x);
/// Derivative of the sigmoidal function
real dsigmoid(real x);
/// The guassian function. Sigma is taken to be 1.0
real gaussian(real x);
/// Derivative of the gaussian function (sigma is taken to be 1.0)
real dgaussian(real x);
/** Signum function
  * Returns real instead of int because this function can be used
  * as an activation function too, so wanted to keep the signature
  * the same as other activation functions.
  * @param x Real value as input
  * @return -1 if x<0, +1 if x>=0
  */
real signum(real x);
/// The tansig activation function. Courtesy Jose Ramos (warta@users.sourceforge.net).
real tansig(real x);
/// Derivative of the tansign activation function. Courtesy Jose Ramos (warta@users.sourceforge.net).
real dtansig(real x);

/// Generates a random real number between -1.0 and 1.0
real random();

/// Generates a random real number between 0.0 and 1.0
real random2();

/** Generates a random integer between given bounds.
  * @param low Lower bound
  * @param high Upper bound
  * @return A random integer i, low<=i<high
  */
int randomInt(int low, int high);

/// Activation functions take in a single real value and return a single real value
/** These functions are used as activation functions of the neurons, i.e., the function 
  * applied to the raw output of the neurons. 
  * @see identity
  * @see sigmoid
  * @see gaussian
  */
typedef real(*ActivationFunction)(real);

/// A set of Links, ie, weighted connections between two neurons
typedef std::vector<Link *> LINKS;

/** One of the fundamental annie classes - the basic Neuron. This class acts only as a 
  * template, and cannot be instantiated. Each neuron has a (unique) label which is specified
  * when creating the neuron. Note that no effort has been made to ensure that the labels
  * are indeed unique, you'll have to take care of that yourself. The MultiLayerNetwork
  * and RadialBasisNetwork for example assign the label of the neuron as:
  * <layer number> * Layer::MAX_LAYER_SIZE + <neuron number in layer>
  *
  * Each Neuron caches the activation, output and error. The getOutput() and getActivation()
  * functions merely check if the cache is valid, if so they return the value in the cache
  * otherwise they force fresh calculation of the output and error.
  * Different types of neurons will have different ways of calculating the output and 
  * error. Hence, the instantiable sub-classes of this class must implement the
  * protected functions _recacheOutput() and _recacheError() which calculate the activation,
  * output and error and update the cache.
  *
  * Also, each sub-class must provide the getClassName() function. Since C++ doesn't have
  * a good class reflection system, this has been provided for some primitive reflection
  *
  * @see InputNeuron
  * @see SimpleNeuron
  * @see CenterNeuron
  */

class Neuron
{
protected:
	/// The label of the neuron
	int _label;

	/** Flag, true if the cached activation and output are valid and hence calls to 
	  * getActivation() and getOutput() can simply return the cached values and don't
	  * have to recalculate.
	  * Set to false by disconnect() and other functions in the sub-classes of this neuron
	  */
	bool _outputCacheValid;

	/// The previously cached activation
	real _activationCache;
	/// The previously cached output
	real _outputCache;

	/** Flag, true if the cached error is valid */
	bool _errorCacheValid;
	/// The previously cached error
	real _errorCache;

	/** The class heirarchy of the neuron. For example, if this list contains
	  * the string "Neuron", "A","B" then it tells you that the particular object
	  * is an instance of class "B", which is a sub-class of "A" which is a sub-class
	  * of Neuron.
	  *
	  * If you ever derive your own Neuron from any of the existing types
	  * of neurons then you MUST add the class name of that neuron to this list.
	  * @see instanceOf
	  */
	std::vector<char *> _classHeirarchy;

	/// Set of input links, i.e., links in which this neuron received input
	LINKS _inputLinks;
public:
	/** Set of output links, i.e., links in which this neuron provides output
	  * This really shouldn't be public, and thus USE THIS AS IF IT WAS PROTECTED.
	  * I had to keep this public because of an implementation issue.
	  */
	LINKS _outputLinks;
protected:
	/// The function that is applied to the activation of the neuron to get the output
	ActivationFunction _activationFunction;
	
	/** This function recalculates output if necessary
	  * Every instantiable sub-class of the basic Neuron class MUST implement this function.
	  * The implementation would first check if the cache is valid and if not then recalculate
	  * the activation and output and update the corresponding cache values. Such a function
	  * would typically look like:
	  * \code
	  * void _recacheOutput()
	  * {
	  *	    if (_outputCacheValid)
	  *	        return;
	  *     //Calculate the activation of the neuron
	  *     _activationCache = activation;
	  *     _outputCache = _activationFunction(_activationCache);
	  *     _outputCacheValid = true;
	  * }
	  * \endcode
	  */
	virtual void _recacheOutput()=0;

	/** This function recalculates error if necessary
	  * Every instantiable sub-class of the basic Neuron class MUST implement this function.
	  * The implementation would first check if the cache is valid and if not then recalculate
	  * the error update the cache value. Such a function would typically look like:
	  * \code
	  * void _recacheError()
	  * {
	  *	    if (_errorCacheValid)
	  *         return;
	  *     //Calculate the error at the neuron
	  *	    _errorCache = error;
	  *	    _errorCacheValid = true;
	  * }
	  * \endcode
	  */
	virtual void _recacheError()=0;
	
public:
	/** Creates a Neuron with the given label, sets activation, output and error to 0
	  * @param label The label to be given to this neuron
	  */
	Neuron(int label);

	Neuron(Neuron &neuron);

	/// Destroys all input and output links
	virtual ~Neuron();

	/** Returns the activation of this neuron
	  *
	  * Calls _recacheOutput() and then returns _activationCache. Most sub-classes will
	  * not need to override this function
	  * @return Activation at this neuron
	  */
	virtual real getActivation();
	
	/** Returns the output of this neuron
	  * Calls _recacheOutput() and then returns _outputCache. Most sub-classes will
	  * not need to override this function
	  * @return Output of this neuron
	  */
	virtual real getOutput();

	/** Returns the error of this neuron
	  * Calls _recacheError() and then returns _errorCache. Most sub-classes will
	  * not need to override this function
	  * @return Error estimate at this neuron
	  */
	virtual real getError();
	
	/// The label of this neuron
	virtual int getLabel();

	/** The size of the input vector taken in by this neuron, ie, the number of neurons that give input to this neuron
	  * @return _inputLinks.size()
	  */
	virtual int getInputCount();

	/** The number of neurons that take the output of this neuron as input
	  * @return _outputLinks.size()
	  */
	virtual int getOutputCount();

	/** Invalidates the output cache of this neuron. Should be called on any structural change
	  * Structural changes such as changes to the input or output links and/or their weights
	  * should invalidate the cache of the neuron using this function.
	  * Causes the cache of all neurons who receive input from this neuron to be invalidated
	  * as well.
	  */
	virtual void invalidateOutputCache();

	/** Invalidates the error cache of this neuron.
	  * Causes the cache of all neurons who provide input to this neuron to be invalidated
	  * as well
	  */
	virtual void invalidateErrorCache();

	/** Returns a list of neurons which provide this neuron with input and their weights
	  * @param labels List of neurons (their labels) that provide input to this neuron returned here
	  * @param weights Weights of the links returned here
	  * Thus, the pair (labels[i],weights[i]) specifies where this neuron gets input from
	  * and what the corresponding weights are
	  */
	virtual int getInputs(std::vector<int> &labels, VECTOR &weights);

	/** Removes the link between the given neuron and this neuron
	  * @param from The neuron which provides input to this neuron
	  * Invalidates the cache as well
	  */
	virtual void disconnect(Neuron *from);

	/** Returns the weight of the link between the given neuron and this
	  * neuron.
	  * @param from The neuron which provides input to this neuron.
	  * @return The weight of the corresponding link
	  */
	virtual real getWeight(Neuron *from);

	/** Formatted string displaying details of this neuron. Useful for debugging.
	  * Sub-classes should implement this function as well in order to display their
	  * characteristics
	  */
	virtual std::string toString();
	
	/** Returns the name of the class that the neuron is an instance of.
	  * This function should be provided by \em every sub-class 
	  */
	virtual const char *getClassName()=0;
	friend class Link;

	/** Given a string, tells if this object is an instance of that class.
	  * Basically, a work-around the absence of standardized class reflection
	  * techniques in C++.
	  * For example, consider the following:
	  * \code
	  * SimpleNeuron n(32);
	  * n.instanceOf("Neuron"); //returns true
	  * n.instanceOf("SimpleNeuron"); //returns true
	  * n.instanceOf("CenterNeuron"); //returns false
	  * \endcode
	  * DO NOT override this function
	  * @param className Name of the class to check this with
	  * @return true if the object is an instance of the given class, false otherwise
	  */
	bool instanceOf(const char *className);

	/** Prints neuron.toString() to the provided output stream
	  * try 
	  * \code cout<<neuron<<endl; \endcode to see what it does
	  */
	friend std::ostream& operator << (std::ostream& s, Neuron &neuron);
};

}; //namespace annie
#endif // define _NEURON_H

