/*
 *	annie - Neural Network Library
 *	http://annie.sourceforge.net/
 *
 *	EXAMPLE - A two layer network that is trained to compute the XOR function.
 *
 *	Last Modified On:
 *		January 12, 2003
 *
 *  Author(s):
 *		Asim Shankar
 *    
 *	A two-layer (one hidden, one output) feed-forward neural network.
 *	2 inputs
 *	3 hidden neurons
 *	1 output
 *	Trained using backpropagation.
 *
 *	This network can be created in two ways, one using a multi-layer network
 *	(MultiLayerNetwork) and building it up, or simply using a TwoLayerNetwork.
 *	This example uses the latter.
 */
#include <iostream>
#include <ctime> 
#include "../include/annie.h"


using namespace std;

//All members of the NeuralNetwork library are in this namespace
using namespace annie;

int main()
{
	//srand((unsigned)time(NULL));
	srand(123);

	//Set the input/output training values
	real input1[]={0,0}; real output1[]={0};
	real input2[]={1,0}; real output2[]={1};
	real input3[]={0,1}; real output3[]={1};
	real input4[]={1,1}; real output4[]={0};

	try
	{
		TwoLayerNetwork net(2,3,1);
		net.connectAll();

		//Create a training set, 2 inputs, 1 output
		TrainingSet T(2,1);
		//Add the various sample data to the training set
		T.addIOpair(input1,output1);
		T.addIOpair(input2,output2);
		T.addIOpair(input3,output3);
		T.addIOpair(input4,output4);

		//Outputs will be placed in this vector
		VECTOR output; 

		cout<<"Results before training:"<<endl;
		output=net.getOutput(input1);	cout<<"0 XOR 0 = "<<output[0]<<endl;
		output=net.getOutput(input2);	cout<<"1 XOR 0 = "<<output[0]<<endl;
		output=net.getOutput(input3);	cout<<"0 XOR 1 = "<<output[0]<<endl;
		output=net.getOutput(input4);	cout<<"1 XOR 1 = "<<output[0]<<endl;

		cout<<"Training the network...please be patient if the no. of epochs is large"<<endl;

		//Train the network using the training set provided
		int epochs=10000;
		net.train(T,10000,0.8);
		cout<<"Results after training "<<epochs<<" epochs"<<endl;
		output=net.getOutput(input1);	cout<<"0 XOR 0 = "<<output[0]<<endl;
		output=net.getOutput(input2);	cout<<"1 XOR 0 = "<<output[0]<<endl;
		output=net.getOutput(input3);	cout<<"0 XOR 1 = "<<output[0]<<endl;
		output=net.getOutput(input4);	cout<<"1 XOR 1 = "<<output[0]<<endl;
	}
	catch (Exception &e)
	{
		cout<<e.what();
	}
	return 0;
}
