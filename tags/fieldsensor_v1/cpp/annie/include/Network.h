#ifndef _NETWORK_H
#define _NETWORK_H

#include "Neuron.h"
#include "InputNeuron.h"
#include "Link.h"
#include <vector>
#include <string>

namespace annie
{

/** Another core class of the annie library, a generic template for a neural network.
  * This class is not instantiable itself. Different types of networks must be derived
  * from this class.
  *
  * The common information held by each network is the number of inputs (i.e.,
  * values that are obtained from the environment), the number of outputs (i.e.,
  * values that are given to the environment) and a string of meta data than can
  * be used by users as pleased.
  * \todo The copy constructor
  */
class Network
{
protected:
	/** Number of inputs taken in by the network. 
	  * There should be these many InputNeurons in the network
	  */
	int _nInputs;
	
	/// Number of outputs given by the network
	int _nOutputs;

	/// A string of meta data, can be used by users as pleased
	std::string _metaData;
public:
	/** Default constructor.
	  *
	  * @param inputs Number of inputs taken in by the network
	  * @param outputs Number of outputs given out by the network
	  */
	Network(int inputs,int outputs);

	/** Copy constructor. NOT YET IMPLEMENTED */
	Network(Network &);

	virtual ~Network();

	/** The output of the network given a stimulus
	  *
	  * Note that instantiable sub-classes of Network <em>must</em>
	  * define this function. How the network determines the output
	  * given the input is what characterizes it.
      * @param input The input given to the network, a VECTOR of size getInputCount()
	  * @return The corresponding output vector (size = getOutputCount())
	  */
	virtual VECTOR getOutput(VECTOR &input)=0;

	/** Wrapper function, calls getOutput(VECTOR &input) */
	virtual VECTOR getOutput(real *input);

	/** Sets the meta data of the network
	  * Meta data is arbitrary text that you may want to tag the network
	  * with. This has no effect on the network output etc
	  */
	virtual void setMetaData(const char *metaData);
	
	/** Sets the meta data of the network
	  * Meta data is arbitrary text that you may want to tag the network
	  * with. This has no effect on the network output etc
	  */
	virtual void setMetaData(std::string metaData);

	/// Returns the meta data string
	virtual std::string getMetaData();

	/// Number of inputs taken in by the network
	virtual int getInputCount();

	/// Number of outputs given by the network
	virtual int getOutputCount();

	/** For reflection.
	  * All sub-classes MUST implement this method, which should
	  * just return class name
	  */
	virtual const char* getClassName()=0;

	/// Finds the name of the network class from the given file
	static std::string getNetworkClassName(const char *filename);

	/** Save the network structure to the given text file.
	  * The file format is quite simple and is commented.
	  * @param filename Name of the file to save network information into
	  */
	virtual void save(const char *filename)=0;
};

}; //namespace annie
#endif // define _NETWORK_H

