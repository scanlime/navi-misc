//////////////////////////////////
// AUTHOR: PATRICK CLARY
// DESCRIPTION:
// implements amc.h
///////////////////////////////////

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include "amc.h"

/************************************** needs to be changed ***************************/
/*** use this just for now ***/
/*** might (definitely) want to make dynamic in the future ***/
#define MAX_AMC_DATA 500000

using namespace std;

amc::amc (char *filename)
{
	a = new amc_data [MAX_AMC_DATA];
	num = 0;
	ifstream in;
	in.open (filename);
	if (in.fail ()) {
		cerr << "Error opening file!" << endl;
		exit (EXIT_FAILURE);
	}

	// strip off junk at beginning a "1" marks the beginning
	string junk;
	do {
		in >> junk;
	} while (junk != "1");

	num = 0;
	int index = 0;
	while (in.peek () != EOF) {
		// input from file
		in >> junk; // get item name
		for (int i = 0; i < 6; i++) in >> a[index].root[i];
		in >> junk;
		for (int i = 0; i < 3; i++) in >> a[index].lowerback[i];
		in >> junk;
		for (int i = 0; i < 3; i++) in >> a[index].upperback[i];
		in >> junk;
		for (int i = 0; i < 3; i++) in >> a[index].thorax[i];
		in >> junk;
		for (int i = 0; i < 3; i++) in >> a[index].lowerneck[i];
		in >> junk;
		for (int i = 0; i < 3; i++) in >> a[index].upperneck[i];
		in >> junk;
		for (int i = 0; i < 3; i++) in >> a[index].head[i];
		in >> junk;
		for (int i = 0; i < 2; i++) in >> a[index].rclavicle[i];
		in >> junk;
		for (int i = 0; i < 3; i++) in >> a[index].rhumerus[i];
		in >> junk;
		in >> a[index].rradius;
		in >> junk;
		in >> a[index].rwrist;
		in >> junk;
		for (int i = 0; i < 2; i++) in >> a[index].rhand[i];
		in >> junk;
		in >> a[index].rfingers;
		in >> junk;
		for (int i = 0; i < 2; i++) in >> a[index].rthumb[i];
		in >> junk;
		for (int i = 0; i < 2; i++) in >> a[index].lclavicle[i];
		in >> junk;
		for (int i = 0; i < 3; i++) in >> a[index].lhumerus[i];
		in >> junk;
		in >> a[index].lradius;
		in >> junk;
		in >> a[index].lwrist;
		in >> junk;
		for (int i = 0; i < 2; i++) in >> a[index].lhand[i];
		in >> junk;
		in >> a[index].lfingers;
		in >> junk;
		for (int i = 0; i < 2; i++) in >> a[index].lthumb[i];
		in >> junk;
		for (int i = 0; i < 3; i++) in >> a[index].rfemur[i];
		in >> junk;
		in >>  a[index].rtibia;
		in >> junk;
		for (int i = 0; i < 2; i++) in >> a[index].rfoot[i];
		in >> junk;
		in >> a[index].rtoes;
		in >> junk;
		for (int i = 0; i < 3; i++) in >> a[index].lfemur[i];
		in >> junk;
		in >> a[index].ltibia;
		in >> junk;
		for (int i = 0; i < 2; i++) in >> a[index].lfoot[i];
		in >> junk;
		in >> a[index].ltoes;

		num++;
		index++;
		// next index
		in >> junk;
	}

	in.close();
}

/////////////////////////////////////////////////////////////////////////////
// splice constructor
// splices second onto first (shifts the root values to maintain continuity)
////////////////////////////////////////////////////////////////////////////
amc::amc (amc first, amc second)
{
	// compute differece for each piece in first element
	amc_data diff;

	// just compute difference for root
	for (int i = 0; i < MAX_ROOT; i++) {
		diff.root[i] = first.a[first.num-1].root[i] - second.a[0].root[i];
	}

	// copy first part over
	a = new amc_data [first.index()+second.index()];
	num = first.index() + second.index();

	/************ might need to do a piece by piece copy ***************/
	// but do this for now //
	for (int i = 0; i < first.index(); i++) {
		a[i] = first.a[i];
	}

	// add second part
	for (int i = 0; i < second.index(); i++) {
		a[first.index()+i] = second.a[i];

		// shift root
		for(int k = 0; k < 3; k++)
			a[first.index()+i].root[k] = second.a[i].root[k] + diff.root[k];
	}
}

void
amc::dumpall ()
{
	for (int i = 0; i < num; i++) {
		cout << i + 1 << endl;
		dumpindex (i);
	};
}

void amc::dumpindex(int index)
{
	cout << "root ";
	for(int i = 0; i < 6; i++) { cout << a[index].root[i] << " "; }
	cout << endl;

	cout << "lowerback ";
	for(int i = 0; i < 3; i++) { cout << a[index].lowerback[i] << " "; }
	cout << endl;

	cout << "upperback ";
	for(int i = 0; i < 3; i++) { cout << a[index].upperback[i] << " "; }
	cout << endl;

	cout << "thorax ";
	for(int i = 0; i < 3; i++) { cout << a[index].thorax[i] << " "; }
	cout << endl;

	cout << "lowerneck ";
	for(int i = 0; i < 3; i++) { cout << a[index].lowerneck[i] << " "; }
	cout << endl;

	cout << "upperneck ";
	for(int i = 0; i < 3; i++) { cout << a[index].upperneck[i] << " "; }
	cout << endl;

	cout << "head ";
	for(int i = 0; i < 3; i++) { cout << a[index].head[i] << " "; }
	cout << endl;

	cout << "rclavicle ";
	for(int i = 0; i < 2; i++) { cout << a[index].rclavicle[i] << " "; }
	cout << endl;

	cout << "rhumerus ";
	for(int i = 0; i < 3; i++) { cout << a[index].rhumerus[i] << " "; }
	cout << endl;

	cout << "rradius ";
	cout << a[index].rradius << endl;

	cout << "rwrist ";
	cout << a[index].rwrist << endl;

	cout << "rhand ";
	for(int i = 0; i < 2; i++) { cout << a[index].rhand[i] << " "; }
	cout << endl;

	cout << "rfingers ";
	cout << a[index].rfingers << endl;

	cout << "rthumb ";
	for(int i = 0; i < 2; i++) { cout << a[index].rthumb[i] << " "; }
	cout << endl;

	cout << "lclavicle ";
	for(int i = 0; i < 2; i++) { cout << a[index].lclavicle[i] << " "; }
	cout << endl;

	cout << "lhumerus ";
	for(int i = 0; i < 3; i++) { cout << a[index].lhumerus[i] << " "; }
	cout << endl;

	cout << "lradius ";
	cout << a[index].lradius << endl;

	cout << "lwrist ";
	cout << a[index].lwrist << endl;

	cout << "lhand ";
	for(int i = 0; i < 2; i++) { cout << a[index].lhand[i] << " "; }
	cout << endl;

	cout << "lfingers ";
	cout << a[index].lfingers << endl;

	cout << "lthumb ";
	for(int i = 0; i < 2; i++) { cout << a[index].lthumb[i] << " "; }
	cout << endl;

	cout << "rfemur ";
	for(int i = 0; i < 3; i++) { cout << a[index].rfemur[i] << " "; }
	cout << endl;

	cout << "rtibia ";
	cout <<  a[index].rtibia << endl;

	cout << "rfoot ";
	for(int i = 0; i < 2; i++) { cout << a[index].rfoot[i] << " "; }
	cout << endl;

	cout << "rtoes ";
	cout << a[index].rtoes << endl;

	cout << "lfemur ";
	for(int i = 0; i < 3; i++) { cout << a[index].lfemur[i] << " "; }
	cout << endl;

	cout << "ltibia ";
	cout << a[index].ltibia << endl;

	cout << "lfoot ";
	for(int i = 0; i < 2; i++) { cout << a[index].lfoot[i] << " "; }
	cout << endl;

	cout << "ltoes ";
	cout << a[index].ltoes << endl;
}
