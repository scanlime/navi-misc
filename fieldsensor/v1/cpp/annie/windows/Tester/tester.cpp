#include "../../include/annie.h"

#include <iostream>
#include <string>
#include <cstdio>
#include <vector>
#include <cmath>
using namespace std;
using namespace annie;


real input1[2]={0,0};
real input2[2]={0,1};
real input3[2]={1,0};
real input4[2]={1,1};

void facerotation(int samples)
{
	for (int ss = 0; ss<samples; ss++)
	{
		int deg = rand() % 360;
		if (ss == 0)
			deg = 0;
		cout<<deg<<"\t"<<tan(deg*3.1415/180)<<"\t";

		double arr[36];
		for (int i=0; i<36; i++)
			arr[i] = cos(3.1415 * (deg - 10*i)/180);

		double x,y;
		x=y=0.0;
		for (int i=0;i<36;i++)
		{
			x += arr[i] * cos(10*i*3.1415/180);
			y += arr[i] * sin(10*i*3.1415/180);
		}
		printf("%.3f\t%.3f\t%.3f\t%.3f\n",x,y,y/x,atan(y/x)*180/3.1415);
	}
}
int delta(int i,int j)
{
	if (i==j)
		return 1;
	return 0;
}
void hopfieldTSP()
{
	HopfieldNetwork net(16,true,false);
	int A,B,C,D;
	A=B=C=500;
	D=200;
	for (int i=0;i<16;i++)
		net.setBias(i,4*D);

	int d[4][4] = {
		{0,6,8,5},
		{6,0,10,5},
		{8,10,0,5},
		{5,5,5,0}
	};

	for (int i1=0;i1<16;i1++)
		for (int j1=i1;j1<16;j1++)
		{
			int i,ip,j,jp,jpp,jpm;
			i=i1/4;
			if (i==1)
			{
				cout<<"i1 = "<<i1<<endl;
				cout<<2/4<<endl;
			}
			j=i1%4;
			ip=j1/4;
			jp=j1%4;
			jpp=(jp+1)%4;
			jpm=jp-1;
			if (jpm<0)
				jpm=3;
			if (i==1 && j==2 && ip==2 && jp==3)
				cout<<"k";
			real t1 = -1*A*delta(i,ip)*(1-delta(j,jp))/2;
			real t2 = -1*B*delta(j,jp)*(1-delta(i,ip))/2;
			real t3 = -C/2;
			real t4 = -D*d[i][j]*(delta(j,jpp)+delta(j,jpm))/2;
			real wt = t1+t2+t3+t4;
			if (i==1 && j==2 && ip==2 && jp==3)
				cout<<"WT1 = "<<wt<<endl;
			if (i==1 && j==2 && ip==1 && jp==3)
				cout<<"WT2 = "<<wt<<endl;
			net.setWeight(i1,j1,wt);
		}


}

void printArray(vector<int> array, int size)
{
	for (int i=0;i<size;i++)
		cout<<array[i]<<" ";
	cout<<endl;
}

void printHopfieldEx63(vector<int> vec)
{
	cout<<endl<<"----------Pattern--------------"<<endl;
	for (int i=0;i<25;i++)
	{
		if (i%5==0)
			cout<<endl;
		if (vec[i]==1)
			cout<<"*";
		else
			cout<<".";
	}
	cout<<endl<<"-------------------------------"<<endl;
}

void printHopfieldEx63(int vec[])
{
	cout<<endl<<"----------Pattern--------------"<<endl;
	for (int i=0;i<25;i++)
	{
		if (i%5==0)
			cout<<endl;
		if (vec[i]==1)
			cout<<"*";
		else
			cout<<".";
	}
	cout<<endl<<"-------------------------------"<<endl;
}

void hopfieldEx63()
{
	HopfieldNetwork net(25);

	int pattern1[25] = {-1,-1, 1,-1,-1,
						-1, 1,-1, 1,-1,
						 1,-1, 1,-1, 1,
						-1, 1,-1, 1,-1,
						-1,-1, 1,-1,-1
	};

	int pattern2[25] = { 1, 1,-1, 1, 1,
						 1,-1,-1,-1, 1,
						-1,-1, 1,-1,-1,
						 1,-1,-1,-1, 1,
						 1, 1,-1, 1, 1
	};
	printHopfieldEx63(pattern1);
	printHopfieldEx63(pattern2);

	//cout<<net.getWeightMatrix()<<endl;
	net.addPattern(pattern1);
	//cout<<net.getWeightMatrix()<<endl;
	net.addPattern(pattern2);
	//cout<<net.getWeightMatrix()<<endl;

	int patternNoise1[25] = {
							-1,-1, 1,-1,-1,
							-1, 1,-1, 1,-1,
							 1,-1, 1,-1, 1,
							-1, 1,-1,-1,-1,
							-1,-1, 1,-1,-1
	};

	int patternNoise2[25] = {
							 1, 1,-1, 1, 1,
							-1,-1,-1,-1, 1,
							-1,-1, 1,-1,-1,
							-1,-1,-1,-1, 1,
							 1, 1,-1, 1, 1
	};

	printHopfieldEx63(patternNoise1);
	//net.getOutput(patternNoise1);
	//net.getNextOutput();
	//net.getNextOutput();
	//net.getNextOutput();
	//cout<<"TOTAL TIME = "<<net.getTime()<<endl;
	//printHopfieldEx63(net.getNextOutput());
	//cout<<"TOTAL TIME = "<<net.getTime()<<endl;
	//printHopfieldEx63(net.getNextOutput());
	//cout<<"TOTAL TIME = "<<net.getTime()<<endl;
	//printHopfieldEx63(net.getNextOutput());
	//cout<<"TOTAL TIME = "<<net.getTime()<<endl;
	//printHopfieldEx63(net.getNextOutput());
	//cout<<"TOTAL TIME = "<<net.getTime()<<endl;
	//printHopfieldEx63(net.getNextOutput());
	//cout<<"TOTAL TIME = "<<net.getTime()<<endl;
	//printHopfieldEx63(net.getNextOutput());
	//printHopfieldEx63(net.getNextOutput());
	//cout<<"TOTAL TIME = "<<net.getTime()<<endl;

	//cout<<net.getWeightMatrix();
}

void hopfieldNetworkTester()
{
	HopfieldNetwork net(4);
	int pattern[] = {1,1,1,-1};
	net.addPattern(pattern);

	cout<<net.getSize()<<endl<<net.getPatternCount()<<endl;
	net.getWeightMatrix().print();
	cout<<endl;
	pattern[3]=0;
	//printArray(net.getOutput(pattern),4);
	//printArray(net.getNextOutput(),4);
	//printArray(net.getNextOutput(),4);
	//printArray(net.getNextOutput(),4);
	//printArray(net.getNextOutput(),4);
}

void hopfieldNetworkTester2()
{
	HopfieldNetwork net(3);
	net.setWeight(0,1,-2/3.0);
	net.setWeight(0,2,2/3.0);
	net.setWeight(1,2,-2/3.0);
	net.getWeightMatrix().print();

	int pattern[3];

	pattern[0]=-1;	pattern[1]=-1;	pattern[2]=-1;
	cout<<net.getEnergy(pattern)<<endl;

	pattern[0]=-1;	pattern[1]=-1;	pattern[2]=1;
	cout<<net.getEnergy(pattern)<<endl;
	
	pattern[0]=-1;	pattern[1]=1;	pattern[2]=-1;
	cout<<net.getEnergy(pattern)<<endl;

	pattern[0]=-1;	pattern[1]=1;	pattern[2]=1;
	cout<<net.getEnergy(pattern)<<endl;

	pattern[0]=1;	pattern[1]=-1;	pattern[2]=-1;
	cout<<net.getEnergy(pattern)<<endl;

	pattern[0]=1;	pattern[1]=-1;	pattern[2]=1;
	cout<<net.getEnergy(pattern)<<endl;

	pattern[0]=1;	pattern[1]=1;	pattern[2]=-1;
	cout<<net.getEnergy(pattern)<<endl;

	pattern[0]=1;	pattern[1]=1;	pattern[2]=1;
	cout<<net.getEnergy(pattern)<<endl;

	cout<<"Starting from (1,1,1), this is how things evolve:"<<endl;
	/*printArray(net.getOutput(pattern),3);
	cout<<"Energy = "<<net.getEnergy()<<endl;
	printArray(net.getNextOutput(),3);
	cout<<"Energy = "<<net.getEnergy()<<endl;
	printArray(net.getNextOutput(),3);
	cout<<"Energy = "<<net.getEnergy()<<endl;

	pattern[0]=-1;	pattern[1]=-1;	pattern[2]=-1;
	cout<<"Starting from (-1,-1,-1), this is how things evolve:"<<endl;
	printArray(net.getOutput(pattern),3);
	cout<<"Energy = "<<net.getEnergy()<<endl;
	printArray(net.getNextOutput(),3);
	cout<<"Energy = "<<net.getEnergy()<<endl;
	printArray(net.getNextOutput(),3);
	cout<<"Energy = "<<net.getEnergy()<<endl;*/

}

void recurrentNeuronTester()
{
	InputNeuron in1(0);
	RecurrentNeuron r1(1);
	r1.connect(&in1,1.0);
	r1.connect(&r1,1.0);
	r1.reset(0);
	r1.setActivationFunction(identity,didentity);
	in1.setValue(1);
	cout<<r1.getOutput()<<endl;
	for (int i=0;i<10;i++)
		cout<<r1.getNextOutput()<<endl;
	cout<<r1.getNextOutput()<<endl;
	cout<<r1.getNextOutput()<<endl;
}

void matrixTester()
{
	Matrix M(3,3);
	
	M.elementAt(0,0)=1;
	M.elementAt(0,1)=3;
	M.elementAt(0,2)=3;
	M.elementAt(1,0)=1;
	M.elementAt(1,1)=4;
	M.elementAt(1,2)=3;
	M.elementAt(2,0)=2;
	M.elementAt(2,1)=7;
	M.elementAt(2,2)=7;

	Matrix *T = M.inverse();
	Matrix *I = M.multiply(T);

	cout<<"\nM:\n";	M.print();
	cout<<"\ninverse:\n";T->print();
	cout<<"\nproduct:\n"; I->print();

	delete I;

	delete T;
}

void bigMatrixTester()
{
	Matrix M(300,300);
	int i,j;
	for (i=0;i<300;i++)
	{
		for (j=0;j<300;j++)
			M.elementAt(i,j)=(i==j)?1.0:0.0;
		cout<<i<<endl;
	}
	M.print();
	cout<<"MATRIX CREATED\n";

	Matrix *inv = M.inverse();
	inv->print();
	cout<<endl;
}

real fn1(real x)
{
	return (real)sqrt(x*x+9);
}

void metaDataTester()
{
	char netfile[255],buff[1000];
	cout<<"Network file name : "; cin>>netfile;
	
	MultiLayerNetwork net(netfile);
	cout<<"Meta data:\n"<<net.getMetaData()<<endl;

	cout<<"Assign meta data  : "; cin>>buff;
	net.setMetaData(buff);

	net.save(netfile);
}

void rbfnEE6761Quiz2()
{
	RadialBasisNetwork net(2,2,1);
	real c1[] = {-7,2};
	real c2[] = {-4,-2};
	net.setCenter(0,c1);
	net.setCenter(1,c2);
	net.setCenterActivationFunction(fn1,NULL);
	net.setWeight(0,0,-0.3532);
	net.setWeight(1,0,0.3532);
	
	cout<<net.getOutput(c1)[0]<<endl;
	cout<<net.getOutput(c2)[0]<<endl;
}



void rbfnXOR()
{
	RadialBasisNetwork net(2,2,1);
	real center1[2]={0,0};
	real center2[2]={1,1};
	net.setCenter(0,center1);
	net.setCenter(1,center2);
	net.setWeight(0,0,2.5031);
	net.setWeight(1,0,2.5031);
	net.setBias(0,-1.848);

	cout<<net.getOutput(input1)[0]<<endl;
	cout<<net.getOutput(input2)[0]<<endl;
	cout<<net.getOutput(input3)[0]<<endl;
	cout<<net.getOutput(input4)[0]<<endl;
}

void mlnTest()
{
	MultiLayerNetwork net(2);
	net.addLayer(3);
	net.addLayer(1);
	net.setBias(1,0,2.53702);
	net.setBias(1,1,0.0193572);
	net.setBias(1,2,7.40168);
	net.setBias(2,0,-4.84302);

	net.connect(0,0,0,-6.38146);
	net.connect(0,1,0,-6.35976);
	net.connect(0,0,1,-2.87389);
	net.connect(0,1,1,-2.80739);
	net.connect(0,0,2,-5.00947);
	net.connect(0,1,2,-4.99942);
	net.connect(1,0,0,-9.60544);
	net.connect(1,1,0,-2.41616);
	net.connect(1,2,0,10.4497);

	cout<<net.getOutput(input1)[0]<<endl;
	cout<<net.getOutput(input2)[0]<<endl;
	cout<<net.getOutput(input3)[0]<<endl;
	cout<<net.getOutput(input4)[0]<<endl;
}

void tlnFileTester()
{
	char filename[100];
	char destinationFilename[100];
	cout<<"Enter annie1 network filename : "; cin>>filename;
	cout<<"Enter annie2 network filename : "; cin>>destinationFilename;
	TwoLayerNetwork net(filename);
	net.save(destinationFilename);
}

void mlnXOR()
{
	TrainingSet T(2,1);
	
	real output1[]={0};
	real output2[]={1};
	real output3[]={1};
	real output4[]={0};

	T.addIOpair(input1,output1);
	T.addIOpair(input2,output2);
	T.addIOpair(input3,output3);
	T.addIOpair(input4,output4);

	MultiLayerNetwork net(2);
	net.addLayer(3);
	net.addLayer(1);
	net.connectLayer(0);
	net.connectLayer(1);

	cout<<"BEFORE TRAINING\n";
	cout<<net.getOutput(input1)[0]<<endl;
	cout<<net.getOutput(input2)[0]<<endl;
	cout<<net.getOutput(input3)[0]<<endl;
	cout<<net.getOutput(input4)[0]<<endl;

	net.train(T,10000,0.8);

	cout<<"\nAFTER TRAINING\n";
	cout<<net.getOutput(input1)[0]<<endl;
	cout<<net.getOutput(input2)[0]<<endl;
	cout<<net.getOutput(input3)[0]<<endl;
	cout<<net.getOutput(input4)[0]<<endl;

	cout<<"Saving\n";
	net.save("C:\\net.txt");
	cout<<"Loading in a two layer network\n";
	TwoLayerNetwork twonet("C:\\net.txt");
	cout<<twonet.getOutput(input1)[0]<<endl;
	cout<<twonet.getOutput(input2)[0]<<endl;
	cout<<twonet.getOutput(input3)[0]<<endl;
	cout<<twonet.getOutput(input4)[0]<<endl;

}

void trsetShuffle()
{
	TrainingSet T(1,1);
	
	real in[1],out[1];
	for (int i=0;i<20;i++)
	{
		in[0]=out[0]=(real)i;
		T.addIOpair(in,out);
	}

	cout<<T<<endl;
	
	int b;
	cin>>b;
//	T.shuffle();
	cout<<T<<endl;
}

void trsetTester()
{
	TrainingSet T("delme");
	cout<<T;
	T.save("delme2");
}

void rbfnTrainingWeightsTester()
{
	TrainingSet T(2,1);
	real output1[]={0};
	real output2[]={1};
	real output3[]={1};
	real output4[]={0};

	T.addIOpair(input1,output1);
	T.addIOpair(input2,output2);
	T.addIOpair(input3,output3);
	T.addIOpair(input4,output4);

	RadialBasisNetwork net(2,2,1);
	real center1[2]={0,0};
	real center2[2]={1,1};
	net.setCenter(0,center1);
	net.setCenter(1,center2);

	cout<<"\nBEFORE TRAINING\n";
	cout<<net.getOutput(input1)[0]<<endl;
	cout<<net.getOutput(input2)[0]<<endl;
	cout<<net.getOutput(input3)[0]<<endl;
	cout<<net.getOutput(input4)[0]<<endl;

	net.trainWeights(T);

	cout<<"\nAFTER TRAINING\n";
	cout<<net.getOutput(input1)[0]<<endl;
	cout<<net.getOutput(input2)[0]<<endl;
	cout<<net.getOutput(input3)[0]<<endl;
	cout<<net.getOutput(input4)[0]<<endl;

	cout<<"\nWEIGHTS\n";
	cout<<net.getWeight(0,0)<<endl;
	cout<<net.getWeight(1,0)<<endl;

	cout<<"SAVING TO TEXT FILE\n";
	net.save("c:\\net.txt");

	RadialBasisNetwork net2("c:\\net.txt");
	cout<<"\nAFTER LOADING FROM FILE\n";
	cout<<net2.getOutput(input1)[0]<<endl;
	cout<<net2.getOutput(input2)[0]<<endl;
	cout<<net2.getOutput(input3)[0]<<endl;
	cout<<net2.getOutput(input4)[0]<<endl;

	cout<<"\nCENTERS\n";
	cout<<net2.getCenter(0)[0]<<", "<<net2.getCenter(0)[1]<<endl;
	cout<<net2.getCenter(1)[0]<<", "<<net2.getCenter(1)[1]<<endl;
	cout<<"\nWEIGHTS\n";
	cout<<net2.getWeight(0,0)<<endl;
	cout<<net2.getWeight(1,0)<<endl;
	net2.save("c:\\net2.txt");

}

void rbfnGradientDescentTester()
{
	RadialBasisNetwork net(2,2,1);
	TrainingSet T(2,1);
	real output1[]={0};
	real output2[]={1};
	real output3[]={1};
	real output4[]={0};

	T.addIOpair(input3,output3);
	T.addIOpair(input1,output1);
	T.addIOpair(input2,output2);
	T.addIOpair(input4,output4);
	
	real center1[2]={0,0};
	real center2[2]={1,1};
	net.setCenter(0,center1);
	net.setCenter(1,center2);

	cout<<"\nBEFORE TRAINING\n";
	cout<<net.getOutput(input1)[0]<<endl;
	cout<<net.getOutput(input2)[0]<<endl;
	cout<<net.getOutput(input3)[0]<<endl;
	cout<<net.getOutput(input4)[0]<<endl;

	//net.trainCentersAndWeights(T,20000,0.8);
	net.trainWeights(T);

	cout<<"\nAFTER TRAINING\n";
	cout<<net.getOutput(input1)[0]<<endl;
	cout<<net.getOutput(input2)[0]<<endl;
	cout<<net.getOutput(input3)[0]<<endl;
	cout<<net.getOutput(input4)[0]<<endl;

	cout<<"\nWEIGHTS\n";
	cout<<net.getWeight(0,0)<<endl;
	cout<<net.getWeight(1,0)<<endl;
}


void createXORTrainingSet()
{
	TrainingSet T(2,1);
	real inputs[2],outputs[1];

	inputs[0]=0;	inputs[1]=0;	outputs[0]=0;
	T.addIOpair(inputs,outputs);

	inputs[0]=0;	inputs[1]=1;	outputs[0]=1;
	T.addIOpair(inputs,outputs);

	inputs[0]=1;	inputs[1]=0;	outputs[0]=1;
	T.addIOpair(inputs,outputs);

	inputs[0]=1;	inputs[1]=1;	outputs[0]=0;
	T.addIOpair(inputs,outputs);

	T.save("xor.dat",annie::BINARY_FILE);
}

void matlab2annieTester()
{
	char netFile[255];
	cout<<"Enter Network Filename : "; cin>>netFile;
	TwoLayerNetwork net(netFile);
	net.setActivationFunction(1,tansig,dtansig);
	net.setActivationFunction(2,tansig,dtansig);
	TrainingSet T("xor.dat",annie::BINARY_FILE);
	VECTOR in,des,out;
	T.initialize();

	while (!T.epochOver())
	{
		T.getNextPair(in,des);
		out = net.getOutput(in);
		printf("(%f,%f) : Wanted %f, got %f\n",in[0],in[1],des[0],out[0]);
	}
}

int main(int argc, char **argv)
{
	srand(1234);
	try
	{
		//mlnTest();
		//trsetTester();
		//mlnXOR();
		//TwoLayerNetwork net(argv[1]);
		//net.save(argv[2]);
		//trsetShuffle();
		//metaDataTester();
		//rbfnEE6761Quiz2();	
		//rbfnXOR();
		//rbfnTrainingWeightsTester();
		//rbfnGradientDescentTester();
		//matrixTester();
		//bigMatrixTester();
		//recurrentNeuronTester();
		//hopfieldNetworkTester();
		//hopfieldNetworkTester2();
		//hopfieldEx63();
		//hopfieldTSP();
		//facerotation(10);
		createXORTrainingSet();
		matlab2annieTester();
		
	}
	catch (Exception e)
	{
		cout<<e.what()<<endl;
	}
	return 0;
}
