#ifndef _INPUTNEURON_H
#define _INPUTNEURON_H

#include "Neuron.h"

namespace annie
{
/** Input neurons are slightly special. They don't really do much, just implement
  * the Neuron interface so that other neurons can create Links to these.
  * Input neurons allow you to set their output. The activation and output of these
  * neurons is the same and equal to the value set.
  */
class InputNeuron : public Neuron
{
private:
	/// Does nothing
	void _recacheOutput();

	/// Does nothing
	void _recacheError();
public:
	/** Creates an input neuron with the given label 
	  * @param label The label to be given to the input
	  */
	InputNeuron(int label);


	/** Sets the input. The result of getOutput() and getActivation() will
	  * be this value.
	  * @param value The input value this neuron is associated with
	  */
	void setValue(real value);

	/** A description of the input. Useful for debugging.
	  * @return A descriptive string. This is just for possible debugging, isn't of
	  *			much other value
	  */
	virtual std::string toString();

	/** For reflection.
	  * @return "InputNeuron
	  */
	virtual const char *getClassName();
};

}; //namespace annie
#endif // define _INPUTNEURON_H

