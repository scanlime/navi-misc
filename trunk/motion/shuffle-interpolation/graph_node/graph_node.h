///////////////////////////////////////////////////
// file : graph_node.h
// author: pclary
// description: defines graph node class
// for use in amc_graph class and with graph edge class
////////////////////////////////////////////////////////

#ifndef GRAPH_NODE_H
#define GRAPH_NODE_H

using namespace std;

#include <stdlib.h>
#include "../graph_edge/graph_edge.h"
#include <vector>

class graph_node
{
public:
	graph_node ();
	graph_node (int node_name, vector <double> min, vector <double> max);

	void add_edge (int to_node_name);
	void visited_again ();

	void clear (); // clears node

	void set (graph_node a); // sets all members to those of a (=);

	int name;
	vector <int> allnames;
	vector <graph_edge> edges;
	int max_weights;
	int times_visited;
	int num_edges;
	vector <double> min_range;
	vector <double> max_range;

	double min_gap, max_gap;
};

#endif
