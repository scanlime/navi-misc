#include "../include/RadialBasisNetwork.h"
#include "../include/Exception.h"
#include "../include/Matrix.h"
#include "../include/File.h"

using namespace std;

namespace annie
{
RadialBasisNetwork::RadialBasisNetwork(int inputs, int centers, int outputs) : Network(inputs,outputs)
{
	int i,j;
	_inputLayer = new InputLayer(0,inputs);

	_nCenters = centers;
	_centerLayer = new Layer(1);
	for (i=0;i<_nCenters;i++)
	{
		CenterNeuron *c = new CenterNeuron(Layer::MAX_LAYER_SIZE*1+i,inputs);
		for (j=0;j<inputs;j++)
			c->connect(&_inputLayer->getNeuron(j));
		_centerLayer->addNeuron(c);
	}

    _outputLayer = new Layer(2);
	for (i=0;i<outputs;i++)
	{
		SimpleNeuron *n = new SimpleNeuron(Layer::MAX_LAYER_SIZE*2+i,true);
		n->setActivationFunction(identity,didentity);
		for (j=0;j<centers;j++)
			n->connect(&_centerLayer->getNeuron(j));
		_outputLayer->addNeuron(n);
	}
}

RadialBasisNetwork::RadialBasisNetwork(RadialBasisNetwork &src) : Network(src)
{
	int i,j,lbl;
	int inputs = src._inputLayer->getSize();
	int centers = src._centerLayer->getSize();
	int outputs = src._outputLayer->getSize();
	_inputLayer = new InputLayer(src._inputLayer->getLabel(),src._inputLayer->getSize());

	_nCenters = src._nCenters;
	lbl = src._centerLayer->getLabel();
	_centerLayer = new Layer(lbl);
	for (i=0;i<centers;i++)
	{
		CenterNeuron *c = new CenterNeuron(Layer::MAX_LAYER_SIZE*lbl+i,inputs);
		CenterNeuron &cSrc = (CenterNeuron&)src._centerLayer->getNeuron(i);
		c->setCenter(cSrc.getCenter());
		for (j=0;j<inputs;j++)
			c->connect(&_inputLayer->getNeuron(j));
		_centerLayer->addNeuron(c);		
	}

	lbl = src._outputLayer->getLabel();
	_outputLayer = new Layer(lbl);
	for (i=0;i<outputs;i++)
	{
		SimpleNeuron *n = new SimpleNeuron(Layer::MAX_LAYER_SIZE*lbl+i);
		SimpleNeuron &nSrc = (SimpleNeuron&)src._outputLayer->getNeuron(i);
		n->setBias(nSrc.getBias());
		n->setActivationFunction(identity,didentity);
		for (j=0;j<centers;j++)
		{
			throw Exception("RadialBasisNetwork::RadialBasisNetwork() - Copy constructor not fully implemented");		
		}
		_outputLayer->addNeuron(n);
	}
}

RadialBasisNetwork::RadialBasisNetwork(const char *filename) : Network(0,0)
{
	File file;
	int i,j;
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
			_inputLayer = new InputLayer(0,_nInputs);
		}
		else if (!s.compare("OUTPUTS"))
		{
			_nOutputs=file.readInt();
		    _outputLayer = new Layer(2);
			for (i=0;i<getOutputCount();i++)
			{
				SimpleNeuron *n = new SimpleNeuron(maxLayerSize*2+i,true);
				n->setActivationFunction(identity,didentity);
				_outputLayer->addNeuron(n);
			}
		}
		else if (!s.compare("CENTERS"))
		{
			_nCenters = file.readInt();
			_centerLayer = new Layer(1);
			for (i=0;i<getCenterCount();i++)
			{
				CenterNeuron *n = new CenterNeuron(maxLayerSize*1+i,getInputCount());
				_centerLayer->addNeuron(n);
			}
		}
		else if (!s.compare("CENTER_POINTS"))
		{
			for (i=0;i<getCenterCount();i++)
			{
				CenterNeuron &n = (CenterNeuron&)_centerLayer->getNeuron(i);
				VECTOR center;
				for (j=0;j<getInputCount();j++)
					center.push_back(file.readDouble());
				n.setCenter(center);
			}
		}
		else if (!s.compare("MAX_LAYER_SIZE"))
			maxLayerSize=file.readInt();
		else if (!s.compare("Biases"))
		{
			for (i=0;i<getOutputCount();i++)
			{
				if (file.eof())
					break;
				SimpleNeuron &o = (SimpleNeuron&)_outputLayer->getNeuron(i);
				if (file.readChar()=='t')
					o.setBias(file.readDouble());
				else
					o.removeBias();
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
		else if (!s.compare("Connections"))
		{
			//Connect inputs to centers
			for (i=0;i<getCenterCount();i++)
			{
				CenterNeuron &c = (CenterNeuron&)_centerLayer->getNeuron(i);
				for (j=0;j<getInputCount();j++)
					c.connect(&_inputLayer->getNeuron(j));
			}
			
			//Connect centers to outputs
			for (i=0;i<getOutputCount();i++)
			{
				SimpleNeuron &o = (SimpleNeuron&)_outputLayer->getNeuron(i);
				for (j=0;j<getCenterCount();j++)
					o.connect(&_centerLayer->getNeuron(j),file.readDouble());
			}
		}
		else
			cerr<<getClassName()<<"::"<<getClassName()<<"() - Unrecognized token ("<<s<<"). Ignoring.\n";
	} // while (!file.eof())
	file.close();
}


RadialBasisNetwork::~RadialBasisNetwork()
{
	delete _inputLayer;
	delete _centerLayer;
	delete _outputLayer;
}

VECTOR
RadialBasisNetwork::getCenter(int i)
{
	VECTOR answer;
	try
	{
		answer = ((CenterNeuron&)_centerLayer->getNeuron(i)).getCenter();
	}
	catch (Exception &e)
	{
		string error(getClassName());
		error = error + "::getCenter() - " + e.what();
		throw Exception(error);
	}
	return answer;
}

//CenterNeuron&
//RadialBasisNetwork::getCenterNeuron(int i)
//{
//	try
//	{
//		return (CenterNeuron&)(_centerLayer->getNeuron(i));
//	}
//	catch (Exception &e)
//	{
//		string error(getClassName());
//		error = error + "::getCenterNeuron() - " + e.what();
//		throw Exception(error);
//	}
//}

VECTOR
RadialBasisNetwork::getOutput(VECTOR &input)
{
	try
	{
		_inputLayer->setInput(input);
		return _outputLayer->getOutput();
	}
	catch(Exception e)
	{
		string error(getClassName());
		error = error + "::getOutput() - "+e.what();
		throw Exception(error);
	}
}

void
RadialBasisNetwork::setCenter(int i, VECTOR &center)
{
	try
	{
		CenterNeuron &c = (CenterNeuron&)_centerLayer->getNeuron(i);
		c.setCenter(center);
	}
	catch (Exception &e)
	{
		string error(getClassName());
		error = error + "::setCenter() - " + e.what();
		throw Exception(e);
	}
}

void
RadialBasisNetwork::setCenter(int i, real *center)
{
	try
	{
		CenterNeuron &c = (CenterNeuron&)_centerLayer->getNeuron(i);
		c.setCenter(center);
	}
	catch (Exception &e)
	{
		string error(getClassName());
		error = error + "::setCenter() - " + e.what();
		throw Exception(e);
	}
}

void
RadialBasisNetwork::setWeight(int center, int output, real weight)
{
	try
	{
		CenterNeuron &c = (CenterNeuron&)_centerLayer->getNeuron(center);
		SimpleNeuron &o = (SimpleNeuron&)_outputLayer->getNeuron(output);
		o.connect(&c,weight);
	}
	catch (Exception &e)
	{
		string error(getClassName());
		error = error + "::setWeight() - " + e.what();
		throw Exception(error);
	}
}

real
RadialBasisNetwork::getWeight(int center, int output)
{
	try
	{
		SimpleNeuron &o = (SimpleNeuron&)_outputLayer->getNeuron(output);
		CenterNeuron &c = (CenterNeuron&)_centerLayer->getNeuron(center);
		return o.getWeight(&c);
	}
	catch (Exception &e)
	{
		string error(getClassName());
		error = error + "::getWeight() - " + e.what();
		throw Exception(error);
	}
}

void
RadialBasisNetwork::setBias(int i, real bias)
{
	try
	{
		SimpleNeuron &n = (SimpleNeuron&)(_outputLayer->getNeuron(i));
		n.setBias(bias);
	}
	catch (Exception &e)
	{
		string error(getClassName());
		error = error + "::setBias() - " + e.what();
		throw Exception(e);
	}
}

real
RadialBasisNetwork::getBias(int i)
{
	try
	{
		SimpleNeuron &n = (SimpleNeuron&)(_outputLayer->getNeuron(i));
		return n.getBias();
	}
	catch (Exception &e)
	{
		string error(getClassName());
		error = error + "::setBias() - " + e.what();
		throw Exception(e);
	}
}

VECTOR
RadialBasisNetwork::getOutput(real *input)
{	return Network::getOutput(input);	}

const char *
RadialBasisNetwork::getClassName()
{	return "RadialBasisNetwork";	}

int
RadialBasisNetwork::getCenterCount()
{	return _nCenters;	}

void
RadialBasisNetwork::setCenterActivationFunction(ActivationFunction f,ActivationFunction df)
{
	int i;
	for (i=0;i<getCenterCount();i++)
	{
		CenterNeuron &c = (CenterNeuron&)_centerLayer->getNeuron(i);
		c.setActivationFunction(f,df);
	}
}

void
RadialBasisNetwork::removeBias(int i)
{
	try
	{
		SimpleNeuron &o = (SimpleNeuron&)_outputLayer->getNeuron(i);
		o.removeBias();
	}
	catch (Exception &e)
	{
		string error(getClassName());
		error = error + "::removeBias() - " + e.what();
		throw Exception(error);
	}
}

void
RadialBasisNetwork::trainWeights(TrainingSet &T)
{
	if (T.getInputSize() != getInputCount())
	{
		string error(getClassName());
		error = error + "::trainWeights() - Invalid TrainingSet provided.";
		throw Exception(error);
	}

	int output;
	int i,j;
	int p = T.getSize();		//number of training patters
	int h = getCenterCount();	//number of centers
	VECTOR in,y;
	
	//do for each output
	for (output=0; output<getOutputCount(); output++)
	{
		T.initialize();
		bool hasBias;
		int effectiveH = h;
		SimpleNeuron &outNrn = (SimpleNeuron&)_outputLayer->getNeuron(output);
		if (hasBias = outNrn.hasBias())
			effectiveH++;

		//setup matrices
		Matrix *Y = new Matrix(p, 1);
		Matrix *W = NULL;
		Matrix *V = new Matrix(p, effectiveH);
		Matrix *VT = NULL;
		if (p!=effectiveH)
			VT = new Matrix(effectiveH ,p);

		for (i=0;i<p;i++)
		{
			T.getNextPair(in,y);
			getOutput(in);
			for (j=0;j<h;j++)
			{
				V->elementAt(i,j) = _centerLayer->getNeuron(j).getOutput();
				if (VT)
					VT->elementAt(j,i) = V->elementAt(i,j);
			}
			if (hasBias)
			{
				V->elementAt(i,j) = 1.0;
				if (VT)
					VT->elementAt(j,i) = 1.0;
			}
			Y->elementAt(i,0) = y[output];
		} // for i=[0..p)
		
		if (VT)
		{
			Matrix *VTVinv, *VTY;
			try
			{
				Matrix *VTV;
				VTV = VT->multiply(V);
				VTVinv = VTV->inverse();
				delete VTV;
			}
			catch (Exception &e)
			{
				string error(getClassName());
				error = error + "::trainWeights() - " + e.what();
				throw Exception(error);
			}
			VTY = VT->multiply(Y);
			W = VTVinv->multiply(VTY);
			delete VTVinv;
			delete VTY;
		} // if VT
		else
		{
			Matrix *Vinv;
			try
			{
				Vinv = V->inverse();
			}
			catch (Exception &e)
			{
				string error(getClassName());
				error = error + "::trainWeights() - " + e.what();
				throw Exception(error);
			}
			W = Vinv->multiply(Y);
			delete Vinv;
		}
			
		//set the weights
		for (j=0;j<h;j++)
			setWeight(j,output,W->elementAt(j,0));
		if (hasBias)
			outNrn.setBias(W->elementAt(j,0));

		//cleanup
		delete Y;
		delete W;
		delete V;
		if (VT)
			delete VT;
	} // for (output = 1...total outputs)
}

void
RadialBasisNetwork::save(const char *filename)
{
	ofstream s;
	int i,j;
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
	s<<"RadialBasisNetwork"<<endl;
	s<<"MAX_LAYER_SIZE "<<Layer::MAX_LAYER_SIZE<<endl;
	s<<"INPUTS "<<getInputCount()<<endl;
	s<<"CENTERS "<<getCenterCount()<<endl;
	s<<"OUTPUTS "<<getOutputCount()<<endl;
	s<<"\nBEGIN_META_DATA"<<endl;
	if (!getMetaData().empty())
		s<<getMetaData()<<endl;
	s<<"END_META_DATA\n"<<endl;
	s<<endl;
	s<<"# Below follows description of the centers. One center per line."<<endl;
	s<<"# Each center is a point in d-dimensional space where d = the number of inputs"<<endl;
	s<<"# ----------------------------------------------------------------------------"<<endl;
	s<<"CENTER_POINTS"<<endl;
	for (i=0; i< getCenterCount(); i++)
	{
		CenterNeuron &c = (CenterNeuron&)_centerLayer->getNeuron(i);
		VECTOR center = c.getCenter();
		VECTOR::iterator it;
		for (it = center.begin(); it!=center.end(); it++)
			s<<(*it)<<" ";
		s<<endl;
	}
	s<<endl;
	s<<"# Now the biases for each of the outputs. One line per output."<<endl;
	s<<"# Line starts with a 't' followed by the bias if the neuron has a bias"<<endl;
	s<<"# otherwise line has an 'f' indicating no bias"<<endl;
	s<<"# ----------------------------------------------------------------------------"<<endl;
	s<<"Biases"<<endl;
	for (i=0;i<getOutputCount(); i++)
	{
		SimpleNeuron &o = (SimpleNeuron&)_outputLayer->getNeuron(i);
		if (o.hasBias())
			s<<"t "<<o.getBias();
		else
			s<<"f ";
		s<<endl;
	}
	s<<endl;
	s<<"# Now follows the weights from the centers to each output."<<endl;
	s<<"# One line for each output. d weights in each line where d = number of centers"<<endl;
	s<<"# ----------------------------------------------------------------------------"<<endl;
	s<<"Connections"<<endl;
	for (i=0; i< getOutputCount(); i++)
	{
		for (j=0;j<getCenterCount();j++)
			s<<getWeight(j,i)<<" ";
		s<<endl;
	}
	s.close();
}


//TRAINING USING GRADIENT DESCENT
//NOT IMPLEMENTED AS OF NOW
//void
//RadialBasisNetwork::trainCentersAndWeights(TrainingSet &T, int epochs, real learningRate)
//{
//	int jj;
//	for (jj=0;jj<getCenterCount();jj++)
//		cout<<_centerLayer->getNeuron(jj).toString()<<endl;
//	for (jj=0;jj<getOutputCount();jj++)
//		cout<<_outputLayer->getNeuron(jj).toString()<<endl;
//
//	int output,i,epoch;
//	VECTOR in,out;
//	for (epoch=0;epoch<epochs;epoch++)
//	{
//		T.initialize();
//		while(!T.epochOver())
//		{
//			T.getNextPair(in,out);
////for (jj=0;jj<in.size();jj++)
////	cout<<in[jj];
////cout<<endl;
//			_inputLayer->setInput(in);
//			//First the weights
//			for (output=0;output<getOutputCount();output++)
//			{
//				SimpleNeuron &o = (SimpleNeuron&)_outputLayer->getNeuron(output);
//				o.setDesiredOutput(out[output]);
//				o.calculateNewWeights(learningRate);
//			} // for (output = 1...total outputs)
//
//			//Now the centers
//			for (i=0;i<getCenterCount();i++) //ith center
//			{
//				CenterNeuron &c = (CenterNeuron&)_centerLayer->getNeuron(i);
//				c.calculateNewCenter(learningRate);
//			}
//
//			//Updates have been calculated, now make them
//			for (i=0;i<getCenterCount();i++) //ith center
//			{
//				CenterNeuron &c = (CenterNeuron&)_centerLayer->getNeuron(i);
//				c.update();
//			}
//			for (output=0;output<getOutputCount();output++)
//			{
//				SimpleNeuron &o = (SimpleNeuron&)_outputLayer->getNeuron(output);
//				o.update();
//			} // for (output = 1...total outputs)
//
//		} // while (!T.epochOver())
//	} // for epochs...
//}

}; //namespace annie

