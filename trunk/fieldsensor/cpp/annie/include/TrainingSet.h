#ifndef _TRAININGSET_H
#define _TRAININGSET_H

#include "Neuron.h"
#include "Network.h"

namespace annie
{

/** This is an abstraction for the set of patterns which are used to
  * "train" a network. It will contain sets of input and corresponding
  * desired-output vectors which will be used by the training algorithms
  * of supervised networks, such as the MultiLayerNetwork and RadialBasisNetwork
  *
  * You can save TrainingSets into a file. The file can be text, a simple format
  * so that you can construct the file from other programs as well, or the file
  * can be a binary file which is understood only by this library. Binary files
  * are generally smaller and quicker to load.
  *
  * You can use utilities provided with the library distribution to convert
  * from one format to the other.
  */
class TrainingSet
{
protected:
	/// The set of input vectors
	std::vector< VECTOR > _inputs;

	/// The set of corresponding desired output vectors
	std::vector< VECTOR > _outputs;

	/// An iterator through the input vectors
	std::vector< VECTOR >::iterator _inputIter;

	/// Iterator through the output vectors
	std::vector< VECTOR >::iterator _outputIter;

	/// Size of an input vector
	int _nInputs;

	/// Size of an output vector
	int _nOutputs;

	/// Save the training set in binary format to the given filename
	void save_binary(const char *filename);

	/// Save the file in text format to the given filename
	void save_text(const char *filename);

	/// Load from a binary file
	void load_binary(const char *filename);

	/// Load from a text file
	void load_text(const char *filename);
public:
	/** Create an empty training set
	  * @param in The size of an input vector
	  * @param out The size of an output vector
	  */
	TrainingSet(int in,int out);

	/** Create a tranining set from a file.
	  * @param filename The name of the file
	  * @param file_type The type of the file (annie::BINARY_FILE) or (annie::TEXT_FILE). TEXT_FILE by default
	  */
	TrainingSet(const char *filename,int file_type = TEXT_FILE);

	virtual ~TrainingSet();

	/// Add an input and correspoding output vector
	void addIOpair(real *input,real *output);

	/** Add an input and corresponding output vector
	  * @throws Exception If the input vector size is not the same as getInputSize()
	  *					or the output vector size is not the same as getOutputSize()
	  */
	void addIOpair(VECTOR input,VECTOR output);

	/** Initialize the training set so that the first call to
	  * getNextPair() gives the first I/O pair stored.
	  * @see getNextPair
	  */
	virtual void initialize();

	/** Has a cycle through all input/output pairs been completed?
	  * @return true if all input/output pairs have been seen through getNextPair
	  * @see getNextPair
	  * @see initialize
	  */
	virtual bool epochOver();

	/// The number of input/desired-output vector pairs in the training set
	virtual int getSize();

	/// The size of an input vector
	virtual int getInputSize();

	/// The size of an output vector
	virtual int getOutputSize();

	/** Returns the next input/output vector pair.
	  * You would typically use this in a fashion somewhat like:
	  * \code
	  * TrainingSet T("trset_file");
	  * T.initialize();
	  * VECTOR in,out;
	  * while (!T.epochOver())
	  * {
	  *		T.getNextPair(in,out);
	  *		// do what you need to with in and out
	  * }
	  * \endcode
	  * @param input The input vector is returned here
	  * @param desired The corresponding desired output vector is returned here
	  */
	virtual void getNextPair(VECTOR &input, VECTOR &desired);

	/// Allows you to print the TrainingSet onto a stream
	friend std::ostream& operator << (std::ostream& s, TrainingSet &T);
	
	/** Saves the training set to a file.
	  * @param filename The name of the file to save the training set to
	  * @param file_type The type of file to generate (annie::BINARY_FILE) or (annie::TEXT_FILE). Default is TEXT_FILE
	  */
	virtual void save(const char *filename, int file_type = TEXT_FILE);

	/// Returns "TrainingSet"
	virtual const char *getClassName();
};

}; //namespace annie
#endif // define _TRAININGSET_H

