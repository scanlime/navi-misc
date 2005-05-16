///////////////////////////////////////////////////
// file : graph_node.cxx
// author: pclary
// description: implements graph node class
// for use in amc_graph class and with graph edge class
////////////////////////////////////////////////////////

using namespace std;

#include <stdlib.h>
#include "graph_node.h"
#include "../graph_edge/graph_edge.h"
#include <vector>

graph_node::graph_node ()
{
	max_weights = 0;
	times_visited = 1;
	num_edges = 0;
}

graph_node::graph_node (int node_name, vector <double> min, vector <double> max)
{
	min_range = min;
	max_range = max;
	max_weights = 0;
	name = node_name;
	times_visited = 1;
}

void
graph_node::add_edge (int to_node_name)
{
	// check if edge to node already exists
	bool exists;
	exists = false;
	graph_edge e;
	unsigned int i;
	max_weights++;

	for (i = 0; i < edges.size(); i++) {
		if (edges[i].to_node_id == to_node_name) {
			exists = true;
			break;
		}
	}

	if(exists) {
		edges[i].weight++;
	} else {
		num_edges++;
		e.to_node_id = to_node_name;
		e.weight = 1;
		edges.push_back (e);
	}
}

void
graph_node::visited_again ()
{
	times_visited++;
}

void graph_node::clear()
{
	vector <graph_edge> e;
	vector <double> min;
	vector <double> max;

	name = -1;
	times_visited = 1;
	max_weights = 0;
	num_edges = 0;
	min_range = min;
	max_range = max;
	edges = e;
}

void
graph_node::set (graph_node a)
{
	// basically just an = operator
	name = a.name;
	edges = a.edges;
	num_edges = a.num_edges;
	max_weights = a.max_weights;
	times_visited = a.times_visited;
	min_range = a.min_range;
	max_range = a.max_range;
}
