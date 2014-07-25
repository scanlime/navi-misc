///////////////////////////////////////////////////////////////////////
// file: shiftsplice.cxx
// author: pclary
// description: this file takes a shuffled amc file and its index.out
// file, which describes shuffle points. It splices/translates, the
// root location (but leaves orientation alone) of the shuffles.
// and sends results to stdout
/////////////////////////////////////////////////////////////////////////

using namespace std;

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
#include "amc.h"

// this index (of amc file!) is the last in a common sequence
vector <int> find_transitions(char *indexfile);

int
main(int argc, char **argv)
{
	if (argc != 3) {
		cerr << "Usage: shiftsplice index.out shuffle.amc" << endl;
		exit(EXIT_FAILURE);
	}

	// find transitions
	vector <int> trans = find_transitions(argv[1]);

	// get shuffled file
	amc shuff(argv[2]);

	// find initial sequence
	amc initial;
	initial.num = trans[0]+1;
	initial.a = new amc_data [shuff.num];

	for (int i = 0; i < initial.num; i++) {
		initial.a[i] = shuff.a[i];
	}

	for (unsigned int transnum = 1; transnum < trans.size(); transnum++) {
		// at transition, create new sequence

		// get next sequence
		amc next;
		unsigned int top;

		if (transnum == trans.size()-1)
			top = shuff.num-1;
		else
			top = trans[transnum];
		next.num = (top - trans[transnum-1]);
		next.a = new amc_data [next.num];

		for(int i = 0; i < next.num; i++)
			next.a[i] = shuff.a[trans[transnum-1]+1+i];

		// splice with old
		amc newsplice(initial,next);

		// and splice to original
		initial.num = newsplice.num;
		for(int i = 0; i < initial.num; i++)
			initial.a[i] = newsplice.a[i];
	}

	// dump sequence
	cout << "#!FILE PROCESSED" << endl;
	cout << ":FULLY-SPECIFIED" << endl;
	cout << ":DEGREES" << endl;

	initial.dumpall();
	return EXIT_SUCCESS;
}


vector <int>
find_transitions (char *indexfile)
{
	vector <int> ans;
	ifstream in;
	in.open (indexfile);
	if (in.fail ()) {
		cerr << "shiftsplice: ERROR OPENING INDEX FILE!" << endl;
		exit(EXIT_FAILURE);
	}

	int prev;
	int next;
	in >> prev;
	int i = 0;
	do {
		in >> next;
		if (prev != (next-1)) {
			// notice, pusing i, not prev
			ans.push_back(i);
		}
		i++;
		prev = next;
	} while (in.peek () != EOF);

	in.close();

	return ans;
}

