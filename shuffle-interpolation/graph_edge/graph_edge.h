///////////////////////////////////////////////////
// file : graph_edge.h
// author: pclary
// description: defines graph edge structure
// for use in amc_graph class and graph node class
///////////////////////////////////////////////////

#include <stdlib.h>
#include "../graph_node/graph_node.h"

#ifndef GRAPH_EDGE_H
#define GRAPH_EDGE_H

struct graph_edge
{
	int to_node_id;
	int weight;
};

#endif
