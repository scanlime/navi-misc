#ifndef _RECURRENTNEURON_H
#define _RECURRENTNEURON_H

#include "SimpleNeuron.h"
#include "defines.h"
#include <vector>

namespace annie
{

/** A neuron used for recurrent networks.
  * These neurons have a concept of time. Thus, their output starts from an initial 
  * state and then as time progresses their output may change.
  *
  * Recurrent networks allow cycles in the graph formed by connections between
  * neurons, which are not allowed by simple multi-layer networks. For example,
  * consider a network in which a recurrent neuron is connected to itself.
  * Output now becomes time dependent.
  * output(time=0) = an initial, fixed value.
  * output(time=1) = weight_of_link * output(0)
  * output(time=t) = weight_of_link * output(t-1) etc.
  *
  * A recurrent neuron has all the features of a simple neuron and adds the concept
  * of time, hence the RecurrentNeuron class is a sub-class of the SimpleNeuron class
  */
class RecurrentNeuron : public SimpleNeuron
{
protected:
	/// Keeps track of the "time", i.e., number of steps taken by this neuron since the last reset
	int _time;

	/// The output of the neuron at time 0
	real _initialOutput;

	/// The last output of the neuron
	real _lastOutput;

	virtual void _recacheOutput();
public:
	/** Creates a recurrent neuron.
	  * The default initial value is 0, thus at time=0 the output
	  * of the neuron will be 0. To change use reset
	  * @param label The label to give to the neuron
	  * @param hasBias Allow this neuron to have a bias?
	  * @see reset
	  */
	RecurrentNeuron(int label, bool hasBias=false);

	/** Updates the time in the neuron by one unit.
	  * This updation will ensure that all other neurons providing input to this
	  * one (directly by a connection or indirectly through a series of connections)
	  * also are brought up to date with the current time
	  */
	virtual void step();

	/** Steps the neuron in time and returns the next output.
	  * @see step
	  * @see getOutput
	  * @see getTime
	  * @return The output of the neuron at the next time step
	  */
	virtual real getNextOutput();

	/** Returns the output of the neuron. At time 0 this will be 
	  * the initial value which is set using reset(), and which is
	  * zero by default. At other times the output will be calculated
	  * @return the output of this neuron at the current time
	  * @see getTime
	  */
	virtual real getOutput();

	/// The output of the neuron at the previous time step. 
	virtual real getLastOutput();

	/// Returns "RecurrentNeuron"
	virtual const char* getClassName();

	/** Returns the current time as known by the neuron. Initially 0, this
	  * value can change by calls to step and getNextOutput
	  * @see step
	  * @see getNextOutput
	  */
	virtual int getTime();

	/** Resets the time to 0.
	  * @param initialActivation The activation of the neuron at time 0. The output at time 0
	  *							 will be the activatation function applied on this value
	  */
	virtual void reset(real initialActivation);

	/// Not yet implemented
	virtual void calculateNewWeights(real learningRate);

	/// Not yet implemented
	virtual void update();

	/// Not yet implemented
	virtual void setDesiredOutput(real desired);
};

}; //namespace annie
#endif // define _RECURRENTNEURON_H

