#include <iostream>
#include <cstring>
#include "../include/annie.h"

using namespace std;
using namespace annie;

#define MAX_FILENAME_SIZE 255

int main(int argc, char *argv[])
{
	cout<<"Usage: "<<argv[0]<<" <binary TrainingSet> <text TrainingSet>"<<endl;
	char inFile[MAX_FILENAME_SIZE];
	char outFile[MAX_FILENAME_SIZE];

	if (argc>1)
	{
		strcpy(inFile,argv[1]);
		cout<<"Enter input filename (binary training set) : "<<inFile<<endl;
	}
	else
	{
        cout<<"Enter input filename (binary training set) : "; cin>>inFile;
	}

	if (argc>2)
	{
		strcpy(outFile,argv[2]);
		cout<<"Enter output filename (text training set)  : "<<outFile<<endl;
	}
	else
	{
		cout<<"Enter output filename (text training set)  : "; cin>>outFile;
	}

	TrainingSet *T;
	try
	{
		T = new TrainingSet(inFile,BINARY_FILE);
		cout<<"TrainingSet of size "<<T->getSize()<<" read."<<endl;
		T->save(outFile,TEXT_FILE);
	}
	catch (Exception e)
	{
		cerr<<e.what()<<endl;
		exit(-1);
	}
	cout<<"Conversion completed successfully"<<endl;
	delete T;

	return 0;
}

