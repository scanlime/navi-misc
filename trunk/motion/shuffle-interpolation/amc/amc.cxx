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
//#define MAX_INDEX 500000
/*** might want to make dynamic in the future ***/
#define MAX_AMC_DATA 500000

using namespace std;
amc::amc(char *filename)
{
    a = new amc_data [MAX_AMC_DATA];
    num = 0;
    ifstream in;
    in.open(filename);
    if(in.fail()) 
    {
        cerr << "Error opening file!" << endl;
        exit(EXIT_FAILURE);
    }
    
    /************************************** needs to be changed ***************************/
    // use this just for now
    //a = new amc_data [MAX_INDEX];
    
    // strip off junk at beginning a "1" marks the beginning
    string junk;
    do
    {
        in >> junk;
          }while(junk != "1");
    
    num=0;
    int index = 0;
    while(in.peek() != EOF)
    {
     // input from file
        
        
        in >> junk; // get item name
        for(int i = 0; i < 6; i++) { in >> a[index].root[i]; }
        
        in >> junk;
        for(int i = 0; i < 3; i++) { in >> a[index].lowerback[i]; }
        
        in >> junk;
        for(int i = 0; i < 3; i++) { in >> a[index].upperback[i]; }
        
        in >> junk;
        for(int i = 0; i < 3; i++) { in >> a[index].thorax[i]; }
        
        in >> junk;
        for(int i = 0; i < 3; i++) { in >> a[index].lowerneck[i]; }
                
        in >> junk;
        for(int i = 0; i < 3; i++) { in >> a[index].upperneck[i]; }
                
        in >> junk;
        for(int i = 0; i < 3; i++) { in >> a[index].head[i]; }
                
        in >> junk;
        for(int i = 0; i < 2; i++) { in >> a[index].rclavicle[i]; }
               
        in >> junk;
        for(int i = 0; i < 3; i++) { in >> a[index].rhumerus[i]; }
                
        in >> junk;
        in >> a[index].rradius;
        
        in >> junk;
        in >> a[index].rwrist;
        
        in >> junk;
        for(int i = 0; i < 2; i++) { in >> a[index].rhand[i]; }
               
        in >> junk;
        in >> a[index].rfingers;
        
        in >> junk;
        for(int i = 0; i < 2; i++) { in >> a[index].rthumb[i]; }
                
        in >> junk;
        for(int i = 0; i < 2; i++) { in >> a[index].lclavicle[i]; }
                
        in >> junk;
        for(int i = 0; i < 3; i++) { in >> a[index].lhumerus[i]; }
                
        in >> junk;
        in >> a[index].lradius;
        
        in >> junk;
        in >> a[index].lwrist;
        
        in >> junk;
        for(int i = 0; i < 2; i++) { in >> a[index].lhand[i]; }
              
        in >> junk;
        in >> a[index].lfingers;
        
        in >> junk;
        for(int i = 0; i < 2; i++) { in >> a[index].lthumb[i]; }
                
        in >> junk;
        for(int i = 0; i < 3; i++) { in >> a[index].rfemur[i]; }
                
        in >> junk;
        in >>  a[index].rtibia;
        
        in >> junk;
        for(int i = 0; i < 2; i++) { in >> a[index].rfoot[i]; }
               
        in >> junk;
        in >> a[index].rtoes;
        
        in >> junk;
        for(int i = 0; i < 3; i++) { in >> a[index].lfemur[i]; }
               
        in >> junk;
        in >> a[index].ltibia;
        
        in >> junk;
        for(int i = 0; i < 2; i++) { in >> a[index].lfoot[i]; }
                
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
// splices second onto first (splice means it shits the root values over)
////////////////////////////////////////////////////////////////////////////
amc::amc(amc first, amc second)
{
    // compute differece for each piece in first element
    amc_data diff;

    // just compute difference for root //
    for(int i = 0; i < MAX_ROOT; i++) { diff.root[i] = first.a[first.num-1].root[i] - second.a[0].root[i]; }
        
    // copy first part over
    a = new amc_data [first.index()+second.index()]; 
    num = first.index()+second.index();
    
    /************ might need to do a piece by piece copy ***************/    
    // but do this for now //
    for(int i = 0; i < first.index(); i++)
    {
        a[i] = first.a[i];
    }
    
    // add second part
    for(int i = 0; i < second.index(); i++)
    {
        a[first.index()+i] = second.a[i];
        
        // shift root
        for(int k = 0; k < 3; k++)
        {
            a[first.index()+i].root[k] = second.a[i].root[k] + diff.root[k];
        }
        
    }
    
    //cout << first.num << " :: " << second.num << " :: " << num << " ::> " << first.num+second.num << endl;
}

void amc::dumpall()
{
    for(int i = 0; i < num; i++) 
    {
        cout << i+1<< endl;
        dumpindex(i);
    };
}

void amc::dumpindex(int index)
{
    //ofstream out;
    //out.open(file.c_string());
    
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

/*

///////////////////////////////////////////////////
// output graph file
// for dot program
// note: numnodes is the number of nodes
// for a 1D graph. there will be more for
// nD graphs.
// ----------------- //
// tbd: also want to save graph in data structure
// possibly use this function to print in future
// but need data structure to interpolate later.
///////////////////////////////////////////////////
void amc::buildgraph(int numnodes) 
{
    // try only for ltoes right now
    
    // find min/max
    int min = 0;
    int max = 0;
    for(int i = 1; i < num; i++)
    {
        if(a[i].ltoes < a[min].ltoes) { min = i; }
        if(a[i].ltoes > a[max].ltoes) { max = i; }
    }
    
    // size of nodes
    double size = (a[max].ltoes-a[min].ltoes)/numnodes;
    
    // go through info.. see what catigories they fit in.. and output
    // find initial categoy
    double lastcgory;
    for(double cgory = size; cgory <= (size*numnodes); cgory += size)
    {
        if(a[0].ltoes <= cgory)
        {
            lastcgory = cgory;
            break;
        }
    }
            
  //  cout << "Digraph {" << endl;
    for(int i = 1; i < MAX_INDEX; i++)
    {
        for(double cgory = size; cgory <= (size*numnodes); cgory += size)
        {
            if(a[i].ltoes <= cgory)
            {
                cout << "\"" << lastcgory << "\"";
                cout << " -> ";
                cout << "\"" << cgory << "\";" << endl;
                lastcgory = cgory;
                break;
            }
        }
    }
//    cout << "}" << endl;
    
}


*/





/**************************************************************/
/*
void amc::creategraph_root(int numnodes, char *filename)
{
    // return if you want to create a graph with no nodes
    if(numnodes <= 0)
    {
        return;
    }
    
    // create outfile stream
    ofstream out;
    out.open(filename);
    if(out.fail())
    {
        cerr << "Error opening file in creategraph: " << filename << endl;
        exit(EXIT_FAILURE);
    }
    
    // find max and mins for root
    double min[MAX_ROOT];
    double max[MAX_ROOT];
    
    // itterate over degrees of freedom
    for(int df = 0; df < MAX_ROOT; df++)
    {
        min[df] = a[0].root[df];
        max[df] = a[0].root[df];
        // itterate over elements in data structure
        for(int e = 1; e < num; e++)
        {
            if(a[e].root[df] < min[df]) { min[df] = a[e].root[df]; }
            if(a[e].root[df] > max[df]) { max[df] = a[e].root[df]; }
        }
    }
    
    double prev_min;
    double prev_max;
    
    double current_min;
    double current_max;
    
    for(int i = 1; i < numnodes; i++)
    {
        if(a[0].root[0] < i*((max[0]-min[0])/numnodes))
        {
            
        }
    }
}
*/
/*
 void creategraph_lowerback(int numnodes, char *filename);
 void creategraph_upperback(int numnodes, char *filename);
 void creategraph_thorax(int numnodes, char *filename);
 void creategraph_lowerneck(int numnodes, char *filename);
 void creategraph_upperneck(int numnodes, char *filename);
 void creategraph_head(int numnodes, char *filename);
 void creategraph_rclavicle(int numnodes, char *filename);
 void creategraph_rhumerus(int numnodes, char *filename);
 */
/*
void amc::creategraph_rradius(int numnodes, char *filename)
{
    // return if 0 numnodes
    if(numnodes <= 0)
    {
        return;
    }
    
    // create file stream
    ofstream out;
    out.open(filename);
    if(out.fail())
    {
        cerr << "Error opening file in creategraph " << filename << endl;
        exit(EXIT_FAILURE);
    }
    
    // find min and max
    double min = a[0].rradius;
    double max = a[0].rradius;
    for(int item = 1; item < num; item++)
    {
        if(a[item].rradius < min) { min = a[item].rradius; }
        if(a[item].rradius > max) { max = a[item].rradius; }
    }
    
    // find seperation space
    double space = (max-min)/numnodes;
    
    // output graph
    int prev;
    int current;
    for(int node =1; node <= numnodes; node++)
    {
        if(a[0].rradius < (node*space))
        {
            prev = node;
            break;
        }
    }
    
    //out << "Digraph {" << endl;
    for(int item = 1; item < num; item++)
    {
        for(int node = 1; node <= numnodes; node++)
        {
            if(a[item].rradius < (node*space))
            {
                current = node;
                out << "\"" << prev << "\"" << " -> " << "\"" << current << "\";" << endl;
                prev = current;
                break;
            }
        }
    }
   // out << "}" << endl << endl;
    out.close();
}


*/
/* 
void creategraph_rwrist(int numnodes, char *filename);
 void creategraph_rhand(int numnodes, char *filename);
 void creategraph_rfingers(int numnodes, char *filename);
 void creategraph_rthumb(int numnodes, char *filename);
 void creategraph_lclavicle(int numnodes, char *filename);
 void creategraph_lhumerus(int numnodes, char *filename);
 void creategraph_lradius(int numnodes, char *filename);
 void creategraph_lwrist(int numnodes, char *filename);
 void creategraph_lhand(int numnodes, char *filename);
 void creategraph_lfingers(int numnodes, char *filename);
 void creategraph_lthumb(int numnodes, char *filename);
 void creategraph_rfemur(int numnodes, char *filename);
 void creategraph_rtibia(int numnodes, char *filename);
 void creategraph_rfoot(int numnodes, char *filename);
 void creategraph_rtoes(int numnodes, char *filename);
 void creategraph_lfemur(int numnodes, char *filename);
 void creategraph_ltibia(int numnodes, char *filename);
 void creategraph_lfoot(int numnodes, char *filename);
 */
/*
void amc::creategraph_ltoes(int numnodes, char *filename)
{
    // return if 0 numnodes
    if(numnodes <= 0)
    {
        return;
    }
    
    // create file stream
    ofstream out;
    out.open(filename);
    if(out.fail())
    {
        cerr << "Error opening file in creategraph " << filename << endl;
        exit(EXIT_FAILURE);
    }
    
    // find min and max
    double min = a[0].ltoes;
    double max = a[0].ltoes;
    for(int item = 1; item < num; item++)
    {
        if(a[item].ltoes < min) { min = a[item].ltoes; }
        if(a[item].ltoes > max) { max = a[item].ltoes; }
    }
    
    // find seperation space
    double space = (max-min)/numnodes;
    
    // output graph
    int prev;
    int current;
    for(int node =1; node <= numnodes; node++)
    {
        if(a[0].ltoes < (node*space))
        {
            prev = node;
            break;
        }
    }
    
    //out << "Digraph {" << endl;
    for(int item = 1; item < num; item++)
    {
        for(int node = 1; node <= numnodes; node++)
        {
            if(a[item].ltoes < (node*space))
            {
                current = node;
                out << "\"" << prev << "\"" << " -> " << "\"" << current << "\";" << endl;
                prev = current;
                break;
            }
        }
    }
    // out << "}" << endl << endl;
    out.close();
}    

*/
/*********************************************************/

/*
 void addtograph_root(int numnodes, char *filename);
 void addtograph_lowerback(int numnodes, char *filename);
 void addtograph_upperback(int numnodes, char *filename);
 void addtograph_thorax(int numnodes, char *filename);
 void addtograph_lowerneck(int numnodes, char *filename);
 void addtograph_upperneck(int numnodes, char *filename);
 void addtograph_head(int numnodes, char *filename);
 void addtograph_rclavicle(int numnodes, char *filename);
 void addtograph_rhumerus(int numnodes, char *filename);
 void addtograph_rradius(int numnodes, char *filename);
 void addtograph_rwrist(int numnodes, char *filename);
 void addtograph_rhand(int numnodes, char *filename);
 void addtograph_rfingers(int numnodes, char *filename);
 void addtograph_rthumb(int numnodes, char *filename);
 void addtograph_lclavicle(int numnodes, char *filename);
 void addtograph_lhumerus(int numnodes, char *filename);
 void addtograph_lradius(int numnodes, char *filename);
 void addtograph_lwrist(int numnodes, char *filename);
 void addtograph_lhand(int numnodes, char *filename);
 void addtograph_lfingers(int numnodes, char *filename);
 void addtograph_lthumb(int numnodes, char *filename);
 void addtograph_rfemur(int numnodes, char *filename);
 void addtograph_rtibia(int numnodes, char *filename);
 void addtograph_rfoot(int numnodes, char *filename);
 void addtograph_rtoes(int numnodes, char *filename);
 void addtograph_lfemur(int numnodes, char *filename);
 void addtograph_ltibia(int numnodes, char *filename);
 void addtograph_lfoot(int numnodes, char *filename);
 void addtograph_ltoes(int numnodes, char *filename);
 */


/*********************************************************/











