#include "../include/TrainingSet.h"
#include "../include/Exception.h"
#include "../include/File.h"
#include "../include/Neuron.h"
#include "../include/defines.h"
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <vector>
using namespace std;
namespace annie
{

TrainingSet::TrainingSet(int in,int out)
{
	this->_nInputs=in;
	this->_nOutputs=out;
}

TrainingSet::~TrainingSet()
{}

void 
TrainingSet::load_text(const char *filename)
{
	File file;
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
	if (s.compare(getClassName()))
	{
		string error(getClassName());
		error = error + "::" + getClassName() + "() - File provided isn't a TrainingSet TEXT_FILE.";
		throw Exception(error);
	}
	while(!file.eof())
	{
		s=file.readWord();
		if (!s.compare("INPUTS"))
			_nInputs=file.readInt();
		else if (!s.compare("OUTPUTS"))
			_nOutputs=file.readInt();
		else if (!s.compare("IO_PAIRS"))
		{
			int j;
			VECTOR input,output;

			while (!file.eof())
			{
				input.clear();
				output.clear();
				for (j=0;j<_nInputs;j++)
					input.push_back(file.readDouble());
				for (j=0;j<_nOutputs;j++)
					output.push_back(file.readDouble());
				_inputs.push_back(input);
				_outputs.push_back(output);
			}
		}
	}
}


void
TrainingSet::load_binary(const char *filename)
{
	ifstream file;
	double version;
	int i;
	file.open(filename,ios::binary);
	if (!file)
		throw Exception("TrainingSet::load_binary() - Couldn't open file for reading");
	file.read((char*)&version,sizeof(version));
	if (version!=atof(ANNIE_VERSION))
		throw Exception("TrainingSet::load_binary() - Invalid training set file encoutered (invalid version)");
	file.read((char*)&_nInputs,sizeof(_nInputs));
	file.read((char*)&_nOutputs,sizeof(_nOutputs));
	_inputs.clear();
	_outputs.clear();
	VECTOR v;
	real tmp;
	while (!file.eof())
	{
		v.clear();
		for (i=0;i<_nInputs;i++)
		{
			file.read((char*)&tmp,sizeof(tmp));
			v.push_back(tmp);
		}
		//Check this!! Why should it be giving EOF on read late?
		if (file.eof())
			break;
        _inputs.push_back(v);
		v.clear();
		for (i=0;i<_nOutputs;i++)
		{
			file.read((char*)&tmp,sizeof(tmp));
			v.push_back(tmp);
		}
		_outputs.push_back(v);
	}
	file.close();
}

TrainingSet::TrainingSet(const char *filename, int file_type)
{
	_nInputs=_nOutputs==0;
	
	if (file_type == annie::TEXT_FILE)
		load_text(filename);
	else if (file_type == annie::BINARY_FILE)
		load_binary(filename);
	//else error
}

void 
TrainingSet::addIOpair(real *input, real *output)
{
	VECTOR in,out;
	int i;
	for (i=0;i<_nInputs;i++)
		in.push_back(input[i]);
	for (i=0;i<_nOutputs;i++)
		out.push_back(output[i]);
	addIOpair(in,out);
}

void
TrainingSet::addIOpair(VECTOR input, VECTOR output)
{
	_inputs.push_back(input);
	_outputs.push_back(output);
}

bool
TrainingSet::epochOver()
{
	if (_inputIter==_inputs.end() && _outputIter==_outputs.end())
		return true;
	return false;
}

void
TrainingSet::initialize()
{
	_inputIter=_inputs.begin();
	_outputIter=_outputs.begin();
}

void
TrainingSet::getNextPair(VECTOR &input, VECTOR &desired)
{
	if (_inputIter==_inputs.end())
	{
		string error(getClassName());
		error = error + "::getNextPair() - Passed the last I/O pair already. No more left.";
		throw Exception(error);
	}
	input=*_inputIter;
	desired=*_outputIter;
	_inputIter++;
	_outputIter++;
}

ostream& operator << (std::ostream& s, TrainingSet &T)
{
	VECTOR::iterator it;
	s<<T.getClassName()<<endl;
	if (s!=cout && s!=cerr)
		s<<"# TrainingSet information"<<endl;
	s<<"INPUTS "<<T._nInputs<<endl;
	s<<"OUTPUTS "<<T._nOutputs<<endl;
	if (s!=cout && s!=cerr)
		s<<"# -------------------------------------------------------- "<<endl;
	s<<"IO_PAIRS"<<endl;
	if (s!=cout && s!=cerr)
	{
		s<<"# -------------------------------------------------------- "<<endl;
		s<<"# Below follow lots of lines for each IO pair - a list of inputs"<<endl;
		s<<"# followed by a list of outputs"<<endl;
		s<<"# The first line will contain a vector with size INPUTS and"<<endl;
		s<<"# the next a vector of size OUTPUTS "<<endl;
	}		
	vector < VECTOR >::iterator ioIn,ioOut;
	for (ioIn=T._inputs.begin(),ioOut=T._outputs.begin();ioIn!=T._inputs.end();ioIn++,ioOut++)
	{
		for (it=ioIn->begin();it!=ioIn->end();it++)
			s<<(*it)<<endl;
		s<<endl;
		for (it=ioOut->begin();it!=ioOut->end();it++)
			s<<(*it)<<endl;
		s<<endl;
		s<<endl;
	}
	return s;
}

void
TrainingSet::save_text(const char *filename)
{
	ofstream file;
	file.open(filename,ios::out);
	if (!file)
		throw Exception("TrainingSet::save_text() - Couldn't open file for writing");
	file<<"ANNIE_FILE ";
	file<<ANNIE_VERSION;
	file<<endl;
	file<<"# Training Set information - the file integrity is"<<endl;
	file<<"# not checked when the file is loaded, so please do"<<endl;
	file<<"# not mess around with the file format as it may cause"<<endl;
	file<<"# errors that will be hard to trace"<<endl;
	file<<(*this);
	file.close();
}

void
TrainingSet::save_binary(const char *filename)
{
	ofstream file;
	file.open(filename,ios::binary);
	if (!file)
		throw Exception("TrainingSet::save_binary() - Couldn't open file for writing");
	double version=atof(ANNIE_VERSION);
	file.write((char*)&version,sizeof(version));
	file.write((char*)&_nInputs,sizeof(_nInputs));
	file.write((char*)&_nOutputs,sizeof(_nOutputs));
	
	vector < VECTOR >::iterator ioIn,ioOut;
	VECTOR::iterator it;
	for (ioIn=_inputs.begin(),ioOut=_outputs.begin();ioIn!=_inputs.end();ioIn++,ioOut++)
	{
		for (it=ioIn->begin();it!=ioIn->end();it++)
			file.write((char*)&(*it),sizeof(*it));
		for (it=ioOut->begin();it!=ioOut->end();it++)
			file.write((char*)&(*it),sizeof(*it));
	}
	file.close();
}

void 
TrainingSet::save(const char *filename, int file_type)
{
	if (file_type == TEXT_FILE)
		save_text(filename);
	else if (file_type == BINARY_FILE)
		save_binary(filename);
	else
	{
		string error(getClassName());
		error = error + "::save() - Invalid file type specified.";
		throw Exception(error);
	}
}


int
TrainingSet::getSize()
{	return _inputs.size();	}

int
TrainingSet::getInputSize()
{	return _nInputs;	}

int
TrainingSet::getOutputSize()
{	return _nOutputs;	}

const char *
TrainingSet::getClassName()
{	return "TrainingSet";	}

//void
//TrainingSet::shuffle()
//{
//	int size = getSize()-1;
//	vector< VECTOR >::iterator inIt,outIt;
//
//	int chosen;
//	while(size>=0)
//	{
//		chosen = (int)(fabs(random())*size);
//		inIt = &_inputs[chosen];
//		outIt = &_outputs[chosen];
//
//		_inputs.push_back(*inIt);
//		_outputs.push_back(*outIt);
//
//		inIt = _inputs.erase(inIt);
//		outIt = _outputs.erase(outIt);
//		size--;
//	}
//}
}; //namespace annie

