///////////////////////////////////////////////////
// file : amc_graph.cxx
// author: pclary
// description: implements amc_graph class
// uses graph_node and graph_edge
///////////////////////////////////////////////////

using namespace std;

#include <stdlib.h>
#include <iomanip>
#include <iostream.h>
#include "amc_graph.h"
#include "../graph_node/graph_node.h"

vector <int> visited; // global visited array for print_graph_helper
void print_graph_helper(vector <graph_node> nodes, 
                        int index);                 // recursive function used in
                                                   // print_graph member function
// constructor
amc_graph::amc_graph()
{
    // stub
}

// destructor
// tbd..

//////// util functions //////////
void amc_graph::print_graph()
{
     if(nodes.size() <= 0)
    { // return if no graph
        cerr << "emtpy graph: nothing to print" << endl;
        return;
    }
    
    cout << "Digraph {" << endl;

    for(unsigned int i = 0; i < nodes.size(); i++)
    {
        for(unsigned int j = 0; j < nodes[i].edges.size(); j++)
        {
            cout << nodes[i].name << " -> " << nodes[i].edges[j].to_node_id << ";" << endl;
        }
    }
   
    for(unsigned int i = 0; i < nodes.size(); i++)
    {
        cout << nodes[i].name << " [label=\""
        << "node: " << nodes[i].name << "\\n" << "visited: " << nodes[i].times_visited
        << "\"];" << endl;
    
    }
    

    
    cout << "}" << endl;
}




void amc_graph::dump_stats()
{
    cout << "num of nodes = " << nodes.size() << endl;
    cout << "df = " << df << endl;
    cout << "nodes:" << endl;
    for(unsigned int i = 0; i < nodes.size(); i++)
    {
        cout <<"index " << i << ": name " << nodes[i].name 
        << ": visited: " << nodes[i].times_visited 
        << "  edges: " << nodes[i].num_edges<< endl;
        cout << "----------"<< endl;
        for(int j = 0; j < df; j++)
        {
            cout << nodes[i].min_range[j] << "\t";
            cout << nodes[i].max_range[j] << endl;
        }
        cout << endl;
    }
    cout << "dfspace:" << endl;
    for(unsigned int i = 0; i < dfspace.size(); i++)
    {
        cout << dfspace[0] << endl;
    }
}

void amc_graph::save_graph(char *filename) // read/save util functions
{
    // stub
}

void amc_graph::read_graph(char *filename) // reads and saves data structures to a data file
{
    // stub
}


struct loc
{
    bool isthere;
    int index; // index in nodes struct, NOT NAME
}; // helper for create graph function


void amc_graph::create_graph(vector <double *> data, vector <int> degf_div, int degf)
{
    // degf_div is the divy for each df
    // degf is the df (as in MAX_(ITEM))
    
    // get max/min for each df
    vector <double> maxs;
    vector <double> mins;
    df = degf;
    dfspace = degf_div;
  //  cerr << "get min/max... ";
    
    for(int i = 0; i < degf; i++)
    {
        maxs.push_back(data[0][i]);
        mins.push_back(data[0][i]);
    }
    for(unsigned int i = 1; i < data.size(); i++)
    {
        for(int j = 0; j < degf; j++)
        {
            if(data[i][j] < mins[j])
            {
                mins[j] = data[i][j];
            }
            if(data[i][j] > maxs[j])
            {
                maxs[j] = data[i][j];
            }
        }
    }
    
    // debugging
    //for(unsigned int i = 0; i < mins.size(); i++)
   // {
    //    cerr << i << ": min = " << mins[i] << ",  max = " << maxs[i] << endl;
   // }
    
   // cerr << "done." << endl;
    
   // cerr << "get widths for each df... ";
    
    // get widths for each df
    vector <double> widths;
    for(int i = 0; i < degf; i++)
    {
        widths.push_back(maxs[i] - mins[i]);
    }
    
    // debugging
   // for(unsigned int i = 0; i < mins.size(); i++)
   // {
   //     cerr << i << ": width = " << widths[i] << endl;
//}
    
   // cerr << "done." << endl;
    
    //cerr << "get gap size for each df... ";
    
    // get gap size for each df
    vector <double> gaps;
    for(int i = 0; i < degf; i++)
    {
        gaps.push_back(widths[i]/degf_div[i]);
    }
    
    // debugging
    //for(unsigned int i = 0; i < mins.size(); i++)
   // {
    //    cerr << i << ": gapsize = " << widths[i] << endl;
   // }
   // cerr << "done." << endl;
    
    //cerr << "get limits for each gap... ";
    
    // get limits for each gap
    // inner array is for df
    vector <vector <double> > limits;
    for(int i = 0; i < degf; i++)
    {
        vector <double> dflimit;
        for(int j = 1; j <= degf_div[i]; j++)
        {
            dflimit.push_back(j*gaps[i]+mins[i]);
        }
        limits.push_back(dflimit);
    }
    
   // for(unsigned int i = 0; i < limits.size(); i++)
    //{
      //  cerr << i << ": limits = ";
      //  for(unsigned int j = 0; j < limits[i].size(); j++)
      //  {
     //       cerr << limits[i][j] << ",";
      //  }
     //   cerr << endl;
   // }
    
   // cerr << "done." << endl;
    
    //cerr << "create isthere var... ";
    
    // variable to check if spot already marked
    vector <vector <loc> > isthere; //inner is DF, outer is gaps
    for(int i = 0; i < degf; i++)
    {
        vector <loc> temp;
        for(int j = 0; j < degf_div[i]; j++)
        {
            loc lt;
            lt.isthere = false;
            lt.index = 0;
            temp.push_back(lt);
        }
        isthere.push_back(temp);
    }
    
    // for debegging.. print isthere
  //  for(unsigned int i = 0; i < isthere.size(); i++)
   // {
    //    for(unsigned int j = 0; j < isthere[i].size(); j++)
     //   {
     //       cerr << "["<< i << "][" << j << "] : ";
    //        cerr << isthere[i][j].isthere << ", " << isthere[i][j].index << endl;
     //   }
  //  }
   // cerr << "***************" << endl;
    //cerr << "done." << endl;
    
    int previdx;
    bool afterfirst = false;
    
    //cerr << "loop and build graph:" << endl;
    
    vector <double> nothing; // used to make min/max_range node function work below
    for(int i = 0; i < degf; i++)
    { nothing.push_back(0.0); }
    
    graph_node node;
    // build graph
    for(unsigned int FRAME = 0; FRAME < data.size(); FRAME++)
    {
      //  cerr << "\t FRAME = " << FRAME << endl;
        for(int DF = 0; DF < degf; DF++)
        {
           // cerr << "\t\t DF = " << DF << endl;
            for(int GAPS = 0; GAPS < degf_div[DF]; GAPS++)
            {
              //  cerr << "\t\t\t GAPS = " << GAPS << " ::\t";
               // cerr << "data["<< FRAME <<"][" << DF << "] = " << data[FRAME][DF] << ",  ";
               // cerr << "limits[" << DF << "][" << GAPS << "] = " << limits[DF][GAPS];
               // cerr << endl;
                // should this frame go here?
                if(data[FRAME][DF] <= limits[DF][GAPS])
                { // yes
                    
                   // cerr << "\t\t\t\t data spot found" << endl;
                    // location already filled
                    if(isthere[DF][GAPS].isthere)
                    {
                        nodes[isthere[DF][GAPS].index].visited_again();
                        nodes[isthere[DF][GAPS].index].allnames.push_back(FRAME);
                        if(afterfirst)
                        {
                            nodes[previdx].add_edge(nodes[isthere[DF][GAPS].index].name);
                        }
                    }
                    else
                    {
                        // create new node
                        isthere[DF][GAPS].isthere = true;
                        isthere[DF][GAPS].index = nodes.size();
                        
                        // vector <double> tmp;
                     //   graph_node node(FRAME,nothing,nothing); // use nothing for now (created above)
                        node.clear();
                        node.name = FRAME;
                        node.allnames.push_back(FRAME);
                        node.min_range = nothing;
                        node.max_range = nothing;
                        node.times_visited = 1;
                        node.num_edges = 0;
                        nodes.push_back(node);
                        
                        // add edge from prev node, if not first node
                        if(afterfirst)
                        {
                            nodes[previdx].add_edge(FRAME);
                        }
                        

                    }
                    
                                        
                    previdx = isthere[DF][GAPS].index;
                    afterfirst = true;
                    goto _FOUND; // break out of 2 loops
                }   
            }
        } _FOUND:; // 
    }
    //cerr << "done: loop and build graph" << endl;
}
    









