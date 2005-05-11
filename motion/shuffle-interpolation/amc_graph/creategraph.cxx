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
    if(argc != 3)
    {
        cerr << "usage: creategraph bone_num amc_file" << endl;
        exit(EXIT_FAILURE);
    }
    
    int bone = (int)atoi(argv[1]);
    amc afile(argv[2]);
    
    vector <double *> *data;
    vector <amc_graph> agraph;
    
   // cerr << "allocated" << endl;
    
    data = convert_all(afile);
    
   // cerr << "converted" << endl;
    
    agraph = create_all_graphs(data);
   // vector <int> n;
   // n.push_back(70);
   // amc_graph a;
// cerr << "creating" << endl;
   // a.create_graph(data[12],n,1);
    
   // cerr << "created" << endl;
   
    if(bone >= (int)agraph.size())
    {
       // cerr << "creategraph: bone not in amc_file" << endl;
       // exit(EXIT_FAILURE);
    }
    
    cerr << "printing bone: " << bone << endl;
    
  //  for(unsigned int i = 0; i < data[bone].size(); i++)
  //  {
   //     cout << data[bone][i][0] << endl;
   // }
    //a.print_graph();
   // a.dump_stats();
    agraph[bone].print_graph();
   // agraph[bone].dump_stats();
    cerr << "printed" << endl;
    return EXIT_SUCCESS;
}



