#include "../include/Network.h"
#include "../include/File.h"
#include <vector>
#include <string>

using namespace std;
namespace annie
{

Network::Network(int inputs, int outputs)
{
	_nInputs = inputs;
	_nOutputs = outputs;
	_metaData.assign("");
}

Network::Network(Network &net)
{
	_nInputs = net._nInputs;
	_nOutputs = net._nOutputs;
}

Network::~Network()
{}

VECTOR
Network::getOutput(real *input)
{
	VECTOR in;
	for (int i=0;i<_nInputs;i++)
		in.push_back(input[i]);
	return getOutput(in);
}

int
Network::getInputCount()
{	return _nInputs;	}

int
Network::getOutputCount()
{	return _nOutputs;	}

void
Network::setMetaData(const char *metaData)
{	_metaData.assign(metaData);	}

void
Network::setMetaData(string metaData)
{	_metaData = metaData;	}

string
Network::getMetaData()
{	return _metaData;	}

string
Network::getNetworkClassName(const char *filename)
{
	File file(filename);
	string answer = file.readWord();
	file.close();
	return answer;
}

}; //namespace annie

