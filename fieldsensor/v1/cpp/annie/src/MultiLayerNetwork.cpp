#include "../include/MultiLayerNetwork.h"
#include "../include/Exception.h"
#include "../include/SimpleNeuron.h"
#include "../include/File.h"

#include <fstream>

using namespace std;
namespace annie
{

MultiLayerNetwork::MultiLayerNetwork(int inputs) : Network(inputs,0)
{
	_nLayers = 0;
	InputLayer *inputLayer = new InputLayer(0,inputs);
	_layers.push_back(inputLayer);
}

MultiLayerNetwork::MultiLayerNetwork(MultiLayerNetwork &src) : Network(src)
{
	throw Exception("MultiLayerNetwork::MultiLayerNetwork() - Copy constructor not yet implemented.");
}

MultiLayerNetwork::MultiLayerNetwork(const char *filename) : Network(0,0)
{	
	_nLayers = 0;
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
	if (s.compare(getClassName())!=0)
	{
		string error(getClassName());
		error = error + "::" + getClassName() + "() - File supplied is not about this type of network.";
		throw Exception(error);
	}

	int maxLayerSize = Layer::MAX_LAYER_SIZE;
	while (!file.eof())
	{
		s=file.readWord();
		if (!s.compare("INPUTS"))
		{
			_nInputs=file.readInt();
			InputLayer *inputLayer=new InputLayer(0,_nInputs);
			_layers.push_back(inputLayer);
		}
		else if (!s.compare("OUTPUTS"))
			_nOutputs=file.readInt();
		else if (!s.compare("MAX_LAYER_SIZE"))
			maxLayerSize=file.readInt();
		else if (!s.compare("SIZE"))
			addLayer(file.readInt());
		else if (!s.compare("Biases"))
		{
			vector < Layer * >::iterator l;
			for (l=_layers.begin()+1;l!=_layers.end();l++)
			{
				Layer *layer = (Layer *)(*l);
				for (int j=0;j<layer->getSize();j++)
				{
					if (file.eof())
						break;
					SimpleNeuron &n = (SimpleNeuron&)layer->getNeuron(j);
					n.setBias(file.readDouble());
				}
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
				metaData.erase(metaData.length()-1); //getting rid of the last "\n"
			setMetaData(metaData);
		}
		else if (!s.compare("Connections"))
			break;
		else
			cerr<<getClassName()<<"::"<<getClassName()<<"() - Unrecognized token ("<<s<<"). Ignoring.\n";
	}
	//Now read connection information
	int label1,label2;
	int layer1,layer2;
	int neuron1,neuron2;
	int nLayers = getLayerCount();
	
	Layer *fromLayer,*toLayer;
	while (!file.eof())
	{
		label1=file.readInt();
		file.readChar();
		label2=file.readInt();
		file.readChar();
		real wt=file.readDouble();
	
		layer1=label1/maxLayerSize;
		layer2=label2/maxLayerSize;
		
		neuron1=label1%maxLayerSize;
		neuron2=label2%maxLayerSize;
		
		if (layer1<0 || layer2<=0 || layer1>=nLayers || layer2>nLayers)
		{
			string error(getClassName());
			error = error + "::" + getClassName() + "() - Error in file. Label of neuron corresponds to an invalid layer.";
			throw Exception(error);
		}
		if (layer2-layer1!=1)
		{
			string error(getClassName());
			error = error + "::" + getClassName() + "() - Error in file. Connection not betwen ith and (i+1)th layer.";
			throw Exception(error);
		}
		
		fromLayer=_layers[layer1];
		toLayer=_layers[layer2];
		
		if (neuron1>=fromLayer->getSize() || neuron2>=toLayer->getSize())
		{
			string error(getClassName());
			error = error + "::" + getClassName() + "() - Error in file. There aren't that many neurons in the layer.";
			throw Exception(error);
		}
		connect(layer1,neuron1,neuron2,wt);
	} // while (!file.eof())
	file.close();
}

MultiLayerNetwork::~MultiLayerNetwork()
{
	vector<Layer *>::iterator it;
	while (!_layers.empty())
	{
		it = _layers.begin();
		Layer *l = (Layer *)(*it);
		delete l;
		it = _layers.erase(it);
	}
}

int
MultiLayerNetwork::getLayerCount()
{	return _nLayers;	}

VECTOR
MultiLayerNetwork::getOutput(real *input)
{	return Network::getOutput(input);	}

VECTOR
MultiLayerNetwork::getOutput(VECTOR &input)
{
	if (getLayerCount()==0)
	{
		string error(getClassName());
		error = error + "::getOutput() - There is no output layer";
		throw Exception(error);
	}
	_inputLayer()->setInput(input);
	try
	{
		return _outputLayer()->getOutput();
	}
	catch (Exception &e)
	{
		string error(getClassName());
		error = error + "::getOutput() - " + e.what();
		throw Exception(e);
	}
}

void
MultiLayerNetwork::addLayer(int size)
{
	_nLayers++;
	int i;
	Layer *newLayer = new Layer(_nLayers);
	for (i=0;i<size;i++)
		newLayer->addNeuron(new SimpleNeuron(_nLayers*Layer::MAX_LAYER_SIZE+i));
	_layers.push_back(newLayer);
	_nOutputs = size;
}

void
MultiLayerNetwork::connectLayer(int src)
{
	Layer *srcLayer, *destLayer;
	int dest = src + 1;
	int srcSize, destSize;
	int i,j;
	if (src<0 || src>getLayerCount()-1)
	{
		string error(getClassName());
		error = error + "::connectLayer() - Invalid source layer specified.";
		throw Exception(error);
	}

	srcLayer = _layers[src];
	destLayer = _layers[dest];
	srcSize = srcLayer->getSize();
	destSize = destLayer->getSize();
	for (i=0;i<destSize;i++)
	{
		SimpleNeuron &destNeuron = (SimpleNeuron&)destLayer->getNeuron(i);
		for (j=0;j<srcSize;j++)
			destNeuron.connect(&srcLayer->getNeuron(j));
	}
}

void
MultiLayerNetwork::connect(int src, int srcNrn, int destNrn, real weight)
{
	Layer *srcLayer, *destLayer;
	int srcSize, destSize;
	if (src<0 || src>getLayerCount()-1)
	{
		string error(getClassName());
		error = error + "::connect() - Invalid source layer specified.";
		throw Exception(error);
	}
	
	srcLayer = _layers[src];
	destLayer = _layers[src+1];
	srcSize = srcLayer->getSize();
	destSize = destLayer->getSize();
	try
	{
		SimpleNeuron &destination = (SimpleNeuron&)destLayer->getNeuron(destNrn);
		destination.connect(&srcLayer->getNeuron(srcNrn),weight);
	}
	catch (Exception &e)
	{
		string error(getClassName());
		error = error + "::connect() - " + e.what();
		throw Exception(e);
	}
}

void
MultiLayerNetwork::connect(int srcLayer, int srcNrn, int destNrn)
{	connect(srcLayer,srcNrn,destNrn,random()); }

void
MultiLayerNetwork::setBias(int layer, int nrn, real bias)
{
	if (layer<0 || layer>getLayerCount())
	{
		string error(getClassName());
		error = error + "::setBias() - Invalid layer specified.";
		throw Exception(error);
	}
	try
	{
		SimpleNeuron &n = (SimpleNeuron&)_layers[layer]->getNeuron(nrn);
		n.setBias(bias);
	}
	catch (Exception &e)
	{
		string error(getClassName());
		error = error + "::setBias() - " + e.what();
		throw Exception(e);
	}
}

InputLayer *
MultiLayerNetwork::_inputLayer()
{	return (InputLayer*)_layers[0];	}

Layer *
MultiLayerNetwork::_outputLayer()
{	return _layers[getLayerCount()];	}

void
MultiLayerNetwork::save(const char *filename)
{
	ofstream s;
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
	s<<"MultiLayerNetwork"<<endl;
	s<<"MAX_LAYER_SIZE "<<Layer::MAX_LAYER_SIZE<<" # There cannot be more than these many neurons in a layer"<<endl;
	s<<"INPUTS "<<getInputCount()<<endl;
	s<<"OUTPUTS "<<getOutputCount()<<endl;
	s<<"\nBEGIN_META_DATA"<<endl;
	if (!getMetaData().empty())
		s<<getMetaData()<<endl;
	s<<"END_META_DATA\n"<<endl;
	s<<"# Below follow layer sizes, the last one is the same as OUTPUTS"<<endl;
	s<<"# -------------------------------------------------------------------------"<<endl;

	vector < Layer * >::iterator it;
	int ctr;
	for (ctr=0,it=_layers.begin()+1;it!=_layers.end();it++,ctr++)
		s<<"SIZE "<<(*it)->getSize()<<" # of layer "<<ctr<<endl;

	s<<"# Now follows information on the neural connections"<<endl;
	s<<"# Basically, MAX_LAYER_SIZE puts an upper bound on the number of neurons in a layer"<<endl;
	s<<"# so connections are specified as (label 1),(label 2),(weight) where "<<endl;
	s<<"# there is a connection from (label 1) --> (label 2) of weight (weight)"<<endl;
	s<<"# The label of neurons is MAX_LAYER_SIZE*L+N where L is the layer number and N is the"<<endl;
	s<<"# neuron number in that layer"<<endl;
	s<<"# Label of input layer is 0, layer below it is 1 and so on"<<endl;

	s<<"Biases"<<endl;
	for (ctr=0,it=_layers.begin()+1;it!=_layers.end();it++,ctr++)
	{
		s<<"# Layer "<<ctr<<", "<<(*it)->getSize()<<" lines follow"<<endl;
		for (int i=0;i<(*it)->getSize();i++)
		{
			SimpleNeuron &n = (SimpleNeuron&)((*it)->getNeuron(i));
			s<<n.getBias()<<endl;
		}
	}

	s<<"Connections"<<endl;
	Neuron *nrn;
	for (it=_layers.begin();it!=_layers.end();it++)
	{
		int layerSize = (*it)->getSize();
		for (ctr=0;ctr<layerSize;ctr++)
		{
			nrn=&((*it)->getNeuron(ctr));
			vector<int> labels;
			vector<real> weights;
			int conns;
			conns=nrn->getInputs(labels,weights);
			for (int j=0;j<conns;j++)
				s<<labels[j]<<","<<nrn->getLabel()<<","<<weights[j]<<endl;
		}
	}
	s.close();
}

void
MultiLayerNetwork::train(TrainingSet &T, int epochs, real learningRate, real momentum)
{	train(T,epochs,learningRate,momentum,false);	}

void
MultiLayerNetwork::train(TrainingSet &T, int epochs, real learningRate, real momentum, bool verbose)
{
	if (T.getInputSize() != getInputCount() || T.getOutputSize() != getOutputCount())
	{
		string error(getClassName());
		error = error + "::train() - Training set not compatible with the network.";
		throw Exception(error);
	}
	
	try
	{
		int epoch;
		for (epoch = 0; epoch < epochs; epoch++)
		{
			if (verbose)
				cerr<<"Begin epoch "<<epoch<<endl;
			T.initialize();
			VECTOR input,desired;
			int i,j;
			int nOutputs = getOutputCount();
			int nLayers = getLayerCount();
			int layerSize;
			while(!T.epochOver())
			{
				T.getNextPair(input,desired);
				_inputLayer()->setInput(input);
				for (i=0;i<nOutputs;i++)
				{
					SimpleNeuron &n = (SimpleNeuron&)_outputLayer()->getNeuron(i);
					n.setDesiredOutput(desired[i]);
				}
				for (i=nLayers;i>0;i--)
				{
					layerSize = _layers[i]->getSize();
					for (j=0; j<layerSize; j++)
					{
						SimpleNeuron &n = (SimpleNeuron&)_layers[i]->getNeuron(j);
						n.calculateNewWeights(learningRate,momentum);
					}
				} //i = layers
				for (i=1;i<=nLayers;i++)
				{
					layerSize = _layers[i]->getSize();
					for (j=0;j<layerSize;j++)
					{
						SimpleNeuron &n = (SimpleNeuron&)_layers[i]->getNeuron(j);
						n.update();
					}
				} //i = layers
			} // while T
		} // epoch
	}
	catch (Exception &e)
	{
		string error(getClassName());
		error = error + "::train() - " + e.what();
		throw Exception(error);
	}
}

void
MultiLayerNetwork::setActivationFunction(int layer, ActivationFunction f, ActivationFunction df)
{
	if (layer <= 0 || layer > _nLayers)
	{
		string error(getClassName());
		error = error + "::setActivationFunction() - Invalid layer provided";
		throw Exception(error);
	}

	int N = _layers[layer]->getSize();
	for (int i=0; i<N; i++)
	{
		SimpleNeuron &n = (SimpleNeuron&)_layers[layer]->getNeuron(i);
		n.setActivationFunction(f,df);
	}
}

const char *
MultiLayerNetwork::getClassName()
{	return "MultiLayerNetwork";	}
}; //namespace annie

