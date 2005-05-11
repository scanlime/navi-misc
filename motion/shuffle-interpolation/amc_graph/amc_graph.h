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
    // constructor
    amc_graph();
    
    // destructor
    // tbd..
    
    //////// util functions
   void create_graph(vector <double *> data, 
                     vector <int> degf_div,
                     int degf);        // vector degf_div
                                       // is an array of ints
                                       // for each df that
                                       // is the ammount that that
                                       // df space is divided up into.

   
   
   void save_graph(char *filename); // read/save util functions
   void read_graph(char *filename); // reads and saves data structures to a data file
   
   void dump_stats();
   void print_graph(); // saves graph in a .dot format
                       // to stdout for now
   

    vector <graph_node> nodes;
    vector <int> dfspace;
    int df;
};



#endif

