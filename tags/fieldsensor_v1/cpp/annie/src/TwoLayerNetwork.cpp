#include "../include/TwoLayerNetwork.h"
#include "../include/SimpleNeuron.h"
#include "../include/Exception.h"

using namespace std;
namespace annie
{
TwoLayerNetwork::TwoLayerNetwork(int inputs, int hidden, int outputs) : MultiLayerNetwork(inputs)
{
	MultiLayerNetwork::addLayer(hidden);
	MultiLayerNetwork::addLayer(outputs);
}

TwoLayerNetwork::TwoLayerNetwork(const char *filename) : MultiLayerNetwork(filename)
{
	if (getLayerCount()!=2)
	{
		string error(getClassName());
		error = error + "::" + getClassName() + "() - The network provided doesn't have 2 layers. Use MultiLayerNetwork instead of " + getClassName() + ".";
		throw Exception(error);
	}
}

void
TwoLayerNetwork::addLayer(int size)
{
	string error(getClassName());
	error = error + "::addLayer() - " + getClassName();
	error = error + " is a restricted class. To use addLayer() use a MultiLayerNetwork instead.";
	throw Exception(error);
}

void
TwoLayerNetwork::connect2in(int input, int hidden, real weight)
{	connect(0,input,hidden,weight);	}

void
TwoLayerNetwork::connect2in(int input, int hidden)
{	connect(0,input,hidden);	}

void
TwoLayerNetwork::connect2out(int hidden, int output, real weight)
{	connect(1,hidden,output,weight);	}

void
TwoLayerNetwork::connect2out(int hidden, int output)
{	connect(1,hidden,output);	}

void
TwoLayerNetwork::connectAll()
{
	connectLayer(0);
	connectLayer(1);
}

const char *
TwoLayerNetwork::getClassName()
{	return "TwoLayerNetwork";	}

}; //namespace annie
