////////////////////////////////////////////////////////////////
// file: creategraph.cxx
// author: pclary
// desc: uses the autogen file ../graph_interp/conversion.h
// and some input to generate various graphs.
// output is sent to stdout.
////////////////////////////////////////////////////////////////

using namespace std;

#include <iostream>
#include <stdlib.h>
#include <vector>
#include "amc_graph.h"
#include "../amc/amc.h"
#include "../graph_interp/conversion.h"

int main(int argc, char **argv)
{
	if(argc != 3) {
		cerr << "usage: creategraph bone_num amc_file" << endl;
		exit(EXIT_FAILURE);
	}

	int bone = (int)atoi(argv[1]);
	amc afile(argv[2]);

	vector<double *> *data;
	vector<amc_graph> agraph;

	data = convert_all(afile);

	agraph = create_all_graphs(data);
	agraph[bone].print_graph();
	return EXIT_SUCCESS;
}
