#include <iostream>
#include <ctime> 
#include <string>
#include "../include/annie.h"

#define MAX_SUM 400

using namespace std;

//All members of the NeuralNetwork library are in this namespace
using namespace annie;

void main()
{
	srand((unsigned)time(NULL));
	int i,j;

	//First generate a training set
	TrainingSet T(2,1);
	real input[2],output[1];
	for (i=0;i<MAX_SUM/2;i+=5)
		for (j=0;j<MAX_SUM/2;j+=3)
		{
			input[0]=static_cast<float>(i)/MAX_SUM;
			input[1]=static_cast<float>(j)/MAX_SUM;
			output[0]=input[0]+input[1];
			T.addIOpair(input,output);
			cout<<input[0]*MAX_SUM<<"+"<<input[1]*MAX_SUM<<"="<<output[0]*MAX_SUM<<endl;
		}

	T.setLearningRate(0.5);
	T.setEpochs(1000);

	//Now create a network - 2 inputs, 4 hidden, 1 output. Completely connected
	Network2Layer net(2,4,1);
	net.connectAll();

	cout<<"Starting training...";
	net.train(T,true);
	cout<<"Training done."<<endl;

	char cont;
	do
	{
		cout<<"Enter number A : ";
		cin>>input[0];
		input[0]/=MAX_SUM;
		cout<<"Enter number B : ";
		cin>>input[1];
		input[1]/=MAX_SUM;
		cout<<"A+B according to the network is "<<(net.output(input)[0])*MAX_SUM;
		cout<<endl<<"Continue?(Y/N)";
		cin>>cont;
	}
	while (cont=='Y' || cont=='y');
}
