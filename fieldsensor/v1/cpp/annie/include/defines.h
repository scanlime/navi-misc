#ifndef _DEFINES_H
#define _DEFINES_H

#include <vector>
#include <string>

#define ANNIE_VERSION "1.0"			
#define MAX_NEURONS_IN_LAYER 1000

#define _SIMPLE_NEURON_STRING "SimpleNeuron"
#define _CENTER_NEURON_STRING "CenterNeuron"
#define _RECURRENT_NEURON_STRING "RecurrentNeuron"
#define _INPUT_NEURON_STRING "InputNeuron"

namespace annie
{
	///Use this instead of double/float for real numbers pertaining to annie
	typedef double real;

	///A vector of real numbers
	typedef std::vector<real> VECTOR;

	/** The TrainingSet can be saved as a binary file or a text file, the latter allowing
	  * users to create a training set without using annie. A binary file is referred to
	  * as annie::BINARY_FILE and a text file as annie::TEXT_FILE
	  * @see TrainingSet
	  * @see TrainingSet::save
	  * @see TEXT_FILE
	  */
	const int BINARY_FILE = 0;

	/** @see BINARY_FILE */
	const int TEXT_FILE = 1;

}

#endif // define _DEFINES_H

