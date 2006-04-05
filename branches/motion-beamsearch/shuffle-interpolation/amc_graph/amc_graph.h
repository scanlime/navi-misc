///////////////////////////////////////////////////
// file : amc_graph.h
// author: pclary
// description: defines amc_graph class
// uses graph_node and graph_edge
////////////////////////////////////////////////////////


#ifndef AMC_GRAPH_H
#define AMC_GRAPH_H

using namespace std;

#include <stdlib.h>
#include <iostream>
#include <vector>
#include "../graph_node/graph_node.h"
#include "../graph_edge/graph_edge.h"

class amc_graph
{
public:
	amc_graph ();

	// degf_div is a list of how many segments each dof in data is partitioned into
	void create_graph (vector <double *> data, vector <int> degf_div, int degf);

	void dump_stats ();

	// prints graph to stdout as .dot
	void print_graph ();

	vector <graph_node> nodes;
	vector <int> dfspace;
	int df;
};

#endif
