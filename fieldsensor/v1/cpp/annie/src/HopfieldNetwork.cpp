#include "../include/HopfieldNetwork.h"
#include "../include/Exception.h"
#include "../include/File.h"
#include <fstream>

using namespace std;
namespace annie
{

real isPositive(real x)
{
	return (real)((x<=(real)0.0)?0.0:1.0);
}

HopfieldNetwork::HopfieldNetwork(int size) : Network(0,size)
{
	int i,j;
	_nPatterns = 0;
	_time = 0;
	_bipolar = true;
	//Add the neurons
	_neurons = new RecurrentNeuron*[size];
	for (i=0; i<size; i++)
	{
		RecurrentNeuron *nrn = new RecurrentNeuron(i,false);
		nrn->reset((real)0.0);
		nrn->setActivationFunction(signum,NULL);
		_neurons[i] = nrn;
	}

	//create an intialize the weight matrix and connect neurons
	_weightMatrix = new Matrix(size,size);
	for (i=0; i<size; i++)
		for (j=0; j<size; j++)
		{
			_weightMatrix->elementAt(i,j)=(real)0.0;
			_neurons[i]->connect(_neurons[j],(real)0.0);
		}
}

HopfieldNetwork::HopfieldNetwork(int size, bool bias, bool bipolar) : Network(0,size)
{
	int i,j;
	_nPatterns = 0;
	_time = 0;
	_bipolar = bipolar;
	
	//create an intialize the weight matrix
	_weightMatrix = new Matrix(size,size);
	for (i=0; i<size; i++)
		for (j=0; j<size; j++)
			_weightMatrix->elementAt(i,j)=(real)0.0;

	//Add the neurons
	_neurons = new RecurrentNeuron*[size];
	for (i=0; i<size; i++)
	{
		RecurrentNeuron *nrn = new RecurrentNeuron(i,bias);
		nrn->reset((real)0.0);
		if (bipolar)
			nrn->setActivationFunction(signum,NULL);
		else
			nrn->setActivationFunction(isPositive,NULL);
		_neurons[i] = nrn;
	}

	//connect the neurons
	for (i=0; i<size; i++)
		for (j=0; j<size; j++)
			_neurons[i]->connect(_neurons[j],_weightMatrix->elementAt(i,j));
}

HopfieldNetwork::HopfieldNetwork(const char *filename) : Network(0,0)
{
	File file;
    int i,j;
	_time = 0;
	try
	{
		file.open(filename);
	}
	catch (Exception &e)
	{
		string error(getClassName());
		error = error + "::" + getClassName() + "() - " + e.what();
		throw Exception(error);
	}
	string s;
	s=file.readWord();
	if (s.compare(getClassName())!=0)
	{
		string error(getClassName());
		error = error + "::" + getClassName() + "() - File supplied is not about this type of network.";
		throw Exception(error);
	}

	_nInputs = 0;
	while (!file.eof())
	{
		s=file.readWord();
		if (!s.compare("SIZE"))
		{
			int size = file.readInt();
			_nOutputs = size;
			_neurons = new RecurrentNeuron*[size];
			_weightMatrix = new Matrix(size,size);
			for (i=0; i<size; i++)
			{
				_neurons[i] = new RecurrentNeuron(i,true);
				if (_bipolar)
					_neurons[i]->setActivationFunction(signum,NULL);
				else
					_neurons[i]->setActivationFunction(isPositive,NULL);
				_neurons[i]->reset((real)0.0);
			}
		}
		else if (!s.compare("WEIGHT_MATRIX"))
		{
			int size = getSize();
			for (i=0; i<size; i++)
				for (j=0;j<size;j++)
				{
					if (file.eof())
					{
						string error(getClassName());
						error = error + "::HopfieldNewtork(filename) - Premature end of file";
						throw Exception(error);
					}
					setWeight(i,j,file.readDouble());
				}
		}
		else if (!s.compare("BIPOLAR"))
			_bipolar=true;
		else if (!s.compare("NOT_BIPOLAR"))
			_bipolar=false;
		else if (!s.compare("Biases"))
		{
			for (i=0;i<getSize();i++)
			{
				if (file.eof())
					break;
				RecurrentNeuron *n = _neurons[i];
				if (file.readChar()=='t')
					n->setBias(file.readDouble());
				else
					n->removeBias();
			}
		}
		else if (!s.compare("BEGIN_META_DATA"))
		{
			static const basic_string <char>::size_type npos = (basic_string <char>::size_type)-1;
			string end("END_META_DATA");
			string metaData;
			s = file.readLine();
			while (s.find(end,0)==npos)
			{
				metaData = metaData + s + "\n";
				s = file.readLine();
			}
			if (metaData.length()>0)
				metaData.erase(metaData.length()-1);
			setMetaData(metaData);
		}
		else
			cerr<<getClassName()<<"::"<<getClassName()<<"() - Unrecognized token ("<<s<<"). Ignoring.\n";
	} // while (!file.eof())
	file.close();
}

HopfieldNetwork::~HopfieldNetwork()
{
	int i;
	delete _weightMatrix;
	for (i=0; i<getSize(); i++)
		delete _neurons[i];
	delete _neurons;
}

const char*
HopfieldNetwork::getClassName()
{	return "HopfieldNetwork";	}

void
HopfieldNetwork::save(const char *filename)
{
	ofstream s;
	int i;
	s.open(filename,ios::out);
	if (!s)
	{
		string error(getClassName());
		error = error + "::save() - Could not open file for writing.";
		throw Exception(error);
	}

	s<<"ANNIE_FILE "<<ANNIE_VERSION<<endl;
	s<<"# Network information, the next line identifies the type of network"<<endl;
	s<<"# Constructing a network from a file which doesn't have the following identifier"<<endl;
	s<<"# should result in an error "<<endl;
	s<<"# DO NOT MAKE FORMAT CHANGES TO THIS FILE"<<endl;
	s<<"HopfieldNetwork"<<endl;
	s<<"SIZE "<<getSize()<<endl;
	if (_bipolar)
		s<<"BIPOLAR"<<endl;
	else
		s<<"NOT_BIPOLAR"<<endl;
	s<<"\nBEGIN_META_DATA"<<endl;
	if (!getMetaData().empty())
		s<<getMetaData()<<endl;
	s<<"END_META_DATA\n"<<endl;
	s<<endl;
	s<<"# Below follows the weight matrix. SIZE lines, one for each row, each"<<endl;
	s<<"# containing SIZE real numbers"<<endl;
	s<<"# ----------------------------------------------------------------------------"<<endl;
	s<<"WEIGHT_MATRIX"<<endl;
	s<<(*_weightMatrix);
	s<<endl;
	s<<"# Biases for the neurons"<<endl;
	s<<"# SIZE lines, one for each neuron. \"t <bias>\" if the neuron has bias, \"f\" otherwise"<<endl;
	s<<"BIASES"<<endl;
	for (i=0; i<getSize(); i++)
	{
		if (_neurons[i]->hasBias())
			s<<"t "<<_neurons[i]->getBias();
		else
			s<<"f";
		s<<endl;
	}
}

VECTOR
HopfieldNetwork::getOutput(VECTOR &input)
{
	int size = input.size();
	if (size != getSize())
	{
		string error(getClassName());
		error = error + "getOutput(VECTOR) - Size of input vector doesn't match size of network";
		throw Exception(error);
	}
	int *intInput = new int[size];
	int i;
	for (i=0; i<size; i++)
		intInput[i]=(int)input[i];

	setInput(intInput);
	delete intInput;
	vector<int> output = getOutput();
	vector<int>::iterator it;
	VECTOR answer;
	for (it = output.begin(); it!=output.end(); it++)
		answer.push_back((real)(*it));
	return answer;
}

void
HopfieldNetwork::setInput(vector<int> &initialState)
{
	_time = 0;
	for (int i=0; i<getSize(); i++)
		_neurons[i]->reset((real)initialState[i]);
}

void
HopfieldNetwork::setInput(int initialState[])
{
	_time = 0;
	for (int i=0; i<getSize(); i++)
		_neurons[i]->reset((real)initialState[i]);
}

void
HopfieldNetwork::step()
{
	//we need to step only one neuron because doing so will cause all others
	//to be stepped anyway
	_neurons[0]->step();
	_time++;
}

vector<int>
HopfieldNetwork::getNextOutput()
{
	step();
	return getOutput();
}

vector<int>
HopfieldNetwork::getOutput()
{
	vector<int> answer;
	for (int i=0;i<_nOutputs;i++)
		answer.push_back((int)_neurons[i]->getOutput());
	return answer;
}

int
HopfieldNetwork::getTime()
{	
	return _time;
}

void
HopfieldNetwork::addPattern(int pattern[])
{
	int i,j;

	//keeping the diagonal of the matrix 0
	int size = getSize();
	for (i=0; i<size; i++)
	{
		if (_bipolar && pattern[i]!=-1 && pattern[i]!=1)
		{
			string error(getClassName());
			error = error + "::addPattern() - Pattern supplied does not consist of only 1 and -1";
			throw Exception(error);
		}
		else if (!_bipolar && pattern[i]!=0 && pattern[i]!=1)
		{
			string error(getClassName());
			error = error + "::addPattern() - Pattern supplied does not consist of only 1 and 0";
			throw Exception(error);
		}

		for (j=i+1; j<size; j++)
		{
			real temp = _weightMatrix->elementAt(i,j);
			temp *= getSize();
			temp += pattern[i]*pattern[j];
			temp /= getSize();
			setWeight(i,j,temp);
		}
	}
	_nPatterns++;
}

int
HopfieldNetwork::getPatternCount()
{	return _nPatterns;	}

int
HopfieldNetwork::getSize()
{	return getOutputCount();	}

void
HopfieldNetwork::setWeight(int i, int j, real weight)
{
	_weightMatrix->elementAt(i,j)=weight;
	_weightMatrix->elementAt(j,i)=weight;
	_neurons[i]->connect(_neurons[j],weight);
	_neurons[j]->connect(_neurons[i],weight);
}

Matrix
HopfieldNetwork::getWeightMatrix()
{	
	Matrix m(*_weightMatrix);
	return m;
}

real
HopfieldNetwork::getEnergy()
{
	real energy = 0.0;
	for (int i=0; i<getSize(); i++)
		for (int j=i; j<getSize(); j++)
		{
			real weight = _weightMatrix->elementAt(i,j);
			real outI = _neurons[i]->getOutput();
			real outJ = _neurons[j]->getOutput();
			energy += weight*outI*outJ;
		}
	return -1*energy;
}

real
HopfieldNetwork::getEnergy(int pattern[])
{
	real energy = 0.0;
	for (int i=0; i<getSize(); i++)
		for (int j=i; j<getSize(); j++)
		{
			real weight = _weightMatrix->elementAt(i,j);
			energy += weight*pattern[i]*pattern[j];
		}
	return -1*energy;
}

void
HopfieldNetwork::setBias(int i, real bias)
{
	if (i>=getSize())
	{
		string error(getClassName());
		error=error+"::setBias() - Invalid neuron number specified";
		throw Exception(error);
	}
	_neurons[i]->setBias(bias);
}

real
HopfieldNetwork::getBias(int i)
{
	if (i>=getSize())
	{
		string error(getClassName());
		error=error+"::getBias() - Invalid neuron number specified";
		throw Exception(error);
	}
	return _neurons[i]->getBias();
}

bool
HopfieldNetwork::propagate(int pattern[], int timeout)
{
	vector<int> p;
	for (int i=0; i<_nOutputs; i++)
		p.push_back(pattern[i]);
	return propagate(p,timeout);
}

bool
HopfieldNetwork::propagate(vector<int> &pattern, int timeout)
{
	setInput(pattern);
	vector<int> last;
	vector<int> curr;
	last = pattern;
	int time = 0;
	while (time < timeout)
	{
		curr = getNextOutput();
		if (_equal(curr,last))
			return true;
		last = curr;
	}
	return false;
}

bool
HopfieldNetwork::_equal(vector<int> &p1, vector<int> &p2)
{
	if (p1.size() != p2.size())
		return false;
	vector<int>::iterator ip1,ip2;
	for (ip1 = p1.begin(), ip2 = p2.begin(); ip1 != p1.end(); ip1++, ip2++)
		if ((*ip1)!=(*ip2))
			return false;
	return true;
}

}; //namespace annie

