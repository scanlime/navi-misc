/*
	Given two Annie Training Sets, merges them together
	(IO pairs of one appended to the other).
	Requires that the training set files be in binary format.
	If not, then use txt2bin to convert them.
*/
#include <annie.h>
#include <iostream>
#include <string>

using namespace std;
using namespace annie;

void printUsage(char *progname);

void main(int argc, char *argv[])
{
	string in1,in2,outfile;
	cout<<"First  training set (binary file) : "; cin>>in1;
	cout<<"Second training set (binary file) : "; cin>>in2;
	cout<<"Merged training set (binary file) : "; cin>>outfile;

	try
	{
		TrainingSet T1(in1.c_str(),annie::BINARY_FILE);
		cout<<"Read "<<in1<<" - "<<T1.getSize()<<endl;
		TrainingSet T2(in2.c_str(),annie::BINARY_FILE);
		cout<<"Read "<<in2<<" - "<<T2.getSize()<<endl;

		if (T1.getInputSize() != T2.getInputSize() || T1.getOutputSize() != T2.getOutputSize())
		{
			cout<<"Input/output counts don't match\nAbort\n";
			exit(-1);
		}
		
		cout<<"Merging....";
		T2.initialize();

		annie::VECTOR in,out;
		while (!T2.epochOver())
		{
			T2.getNextPair(in,out);
			T1.addIOpair(in,out);
		}
		T1.save(outfile.c_str(),annie::BINARY_FILE);
		cout<<"Saved into "<<outfile<<", "<<T1.getSize()<<" IO pairs"<<endl;
	}
	catch (Exception &e)
	{
		cerr<<e.what()<<endl;
		exit(-1);
	}
}