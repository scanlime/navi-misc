#include <iostream>
#include <ctime> 
#include <string>
#include <fstream>
#include "../include/annie.h"

using namespace std;

//All members of the NeuralNetwork library are in this namespace
using namespace annie;

void TrainingSet2SVMLearn();
void nnSVMcompare()
{
	vector<float> in;
	vector<float> out;
	try
	{
		TrainingSet T("C:/tmp/test.txt");
		Network2Layer net("C:/tmp/network.txt");
		T.initialize();
		cout<<T.size()<<endl;
		cout<<"OUTPUTS:"<<endl;
		while (!T.epochOver())
		{
			T.getNextPair(in,out);
			cout<<out[0]<<"\t";
			cout<<net.output(in)[0]<<endl;
		}

	}
	catch (Exception &e)
	{
		cerr<<e.what()<<endl;
	}
}

void main()
{
	TrainingSet2SVMLearn();
	nnSVMcompare();
}

void TrainingSet2SVMLearn()
{
	vector<float> in,out;
	try
	{
		TrainingSet T("C:/tmp/test.txt");
		ofstream ofile("C:/tmp/test_svm.txt",ios::out);
		T.initialize();
		cout<<T.size();

		while (!T.epochOver())
		{
			T.getNextPair(in,out);
			int ctr=0;
			if (out[0]==0.0)
				ofile<<"-1 ";
			else
				ofile<<"+1 ";
			for (int i=0;i<in.size();i++)
				if (in[i]>0.004)
					ofile<<(i+1)<<":"<<in[i]<<" ";
			ofile<<endl;
		}
		ofile.close();
	}
	catch (Exception &e)
	{
		cerr<<e.what()<<endl;
	}
}

void mergeTS_BS()
{
	/*float input[2];
	float output[1];
	input[0]=1.0;
	input[1]=0.5;
	output[0]=0.9;
	TrainingSet T(2,1);
	T.addIOpair(input,output);
	T.addIOpair(input,output);
	T.save("delme");*/
	
	vector<float> in;
	vector<float> out;

	bool bsempty=false;
	bool tsempty=false;
	try
	{
		TrainingSet TS_BS("C:/tmp/TS_BS.txt");
		cout<<TS_BS.size();
		int ctr=0;
		TS_BS.initialize();
		while (!TS_BS.epochOver())
		{
			ctr++;
			if (ctr%100==0)
				cout<<ctr<<endl;
			TS_BS.getNextPair(in,out);
		}
		//TrainingSet BS("C:/tmp/BS.txt");
		//TrainingSet TS("C:/tmp/TS.txt");
		//TrainingSet TS_BS(400,1);

		//TS.initialize();
		//BS.initialize();
		//int c1=0,c2=0;
		//while (!tsempty || !bsempty)
		//{
		//	if (c1%100==0 || c2%100==0)
		//		cout<<c1<<"\t"<<c2<<endl;
		//	if (!tsempty)
		//	{
		//		TS.getNextPair(in,out);
		//		TS_BS.addIOpair(in,out);
		//		TS.getNextPair(in,out);
		//		TS_BS.addIOpair(in,out);
		//		c1+=2;
		//	}
		//	if (!bsempty)
		//	{
		//		BS.getNextPair(in,out);
		//		TS_BS.addIOpair(in,out);
		//		c2++;
		//	}
		//	if (TS.epochOver())
		//		tsempty=true;
		//	if (BS.epochOver())
		//		bsempty=true;
		//}
		//cout<<TS_BS.size()<<endl;
		//TS_BS.save("C:/tmp/TS_BS.txt");
	}
	catch (Exception e)
	{
		cout<<e.what()<<endl;
	}
	//while (!T.epochOver())
	//{
	//	T.getNextPair(in,out);
	//	//cout<<in[0]<<" "<<in[1]<<" "<<out[0]<<endl;
	//}
	
}

void main2()
{
	srand((unsigned)time(NULL));

	//Set the input/output training values
	float input1[]={0,0,1,0,0,0,1,0,1,0,1,0,0,0,1,1,0,0,0,1,1,1,1,1,1,1,0,0,0,1,1,0,0,0,1};
	float input2[]={1,0,0,0,1,0,1,0,1,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,1,0,1,0,1,0,0,0,1};
	float input3[]={1,0,0,0,1,1,0,0,0,1,1,0,0,0,1,1,1,1,1,1,1,0,0,0,1,1,0,0,0,1,1,0,0,0,1};
	float input4[]={1,1,1,1,1,1,0,0,0,1,1,0,0,0,1,1,1,1,1,1,1,0,0,0,1,1,0,0,0,1,1,1,1,1,1};
	float input5[]={0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0};

	float output1[]={0,0,0};
	float output2[]={0,1,0};
	float output3[]={1,0,0};
	float output4[]={1,0,1};
	float output5[]={1,1,1};

	try
	{
		Network2Layer net(35,5,3);
		net.connectAll();
		net.save("tester.txt");
		
		//Create a training set, 2 inputs, 1 output
		TrainingSet T(35,3);
		//Add the various sample data to the training set
		T.addIOpair(input1,output1);
		T.addIOpair(input2,output2);
		T.addIOpair(input3,output3);
		T.addIOpair(input4,output4);
		T.addIOpair(input5,output5);
		//Set the learning rate to be used when training with this set
		T.setLearningRate(0.8);
		//Set the number of epochs
		T.setEpochs(1000);

		
		T.addLearningRate(0.3);
		T.save("trainingset");

		//Outputs will be placed in this vector
		std::vector<float> output; 

		cout<<"Results before training:"<<endl;
		output=net.output(input1);	cout<<"out1 = "<<output[0]<<","<<output[1]<<","<<output[2]<<endl;
		output=net.output(input2);	cout<<"out1 = "<<output[0]<<","<<output[1]<<","<<output[2]<<endl;
		output=net.output(input3);	cout<<"out1 = "<<output[0]<<","<<output[1]<<","<<output[2]<<endl;
		output=net.output(input4);	cout<<"out1 = "<<output[0]<<","<<output[1]<<","<<output[2]<<endl;
		output=net.output(input5);	cout<<"out1 = "<<output[0]<<","<<output[1]<<","<<output[2]<<endl;
		
		cout<<"Training the network...please be patient if the no. of epochs is large"<<endl;

		//Train the network using the training set provided
		net.train(T);
		cout<<"Results after training "<<T.epochs()<<" epochs"<<endl;
		output=net.output(input1);	cout<<"out1 = "<<output[0]<<","<<output[1]<<","<<output[2]<<endl;
		output=net.output(input2);	cout<<"out1 = "<<output[0]<<","<<output[1]<<","<<output[2]<<endl;
		output=net.output(input3);	cout<<"out1 = "<<output[0]<<","<<output[1]<<","<<output[2]<<endl;
		output=net.output(input4);	cout<<"out1 = "<<output[0]<<","<<output[1]<<","<<output[2]<<endl;
		output=net.output(input5);	cout<<"out1 = "<<output[0]<<","<<output[1]<<","<<output[2]<<endl;



		NetworkMultiLayer net2("tester.txt");
		TrainingSet T2("trainingset");
		cout<<"Results from saved file"<<endl;
		output=net2.output(input1);	cout<<"out1 = "<<output[0]<<","<<output[1]<<","<<output[2]<<endl;
		output=net2.output(input2);	cout<<"out1 = "<<output[0]<<","<<output[1]<<","<<output[2]<<endl;
		output=net2.output(input3);	cout<<"out1 = "<<output[0]<<","<<output[1]<<","<<output[2]<<endl;
		output=net2.output(input4);	cout<<"out1 = "<<output[0]<<","<<output[1]<<","<<output[2]<<endl;
		output=net2.output(input5);	cout<<"out1 = "<<output[0]<<","<<output[1]<<","<<output[2]<<endl;
		net2.train(T2);
		cout<<"Results after training "<<T2.epochs()<<" epochs"<<endl;
		output=net2.output(input1);	cout<<"out1 = "<<output[0]<<","<<output[1]<<","<<output[2]<<endl;
		output=net2.output(input2);	cout<<"out1 = "<<output[0]<<","<<output[1]<<","<<output[2]<<endl;
		output=net2.output(input3);	cout<<"out1 = "<<output[0]<<","<<output[1]<<","<<output[2]<<endl;
		output=net2.output(input4);	cout<<"out1 = "<<output[0]<<","<<output[1]<<","<<output[2]<<endl;
		output=net2.output(input5);	cout<<"out1 = "<<output[0]<<","<<output[1]<<","<<output[2]<<endl;
		
	}
	catch (Exception e)
	{
		cout<<e.what();
	}
}

