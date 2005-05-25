#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <vector>
#include <math.h>
#include "vect.h"
#include "runge.h"
#include "../amc/amc.h"

//////////////////////////////////////////////////////////////////////////////
// the binary for this file should be moved to ../bin/shuffle
// this file generates 3 output files (data_d, lorenz_d, index.out)
// as well as sends output to stdout
// lorenz_d: points generated on the attractor function, defined in function.h
// data_d: points of the input file laid out on the attractor
// index.out: the shuffled index of the amc file
// stdout: a complete amc file of shuffled frames
///////////////////////////////////////////////////////////////////////////////



#define DATA_NAME "lorenz_d"
#define INDEX_OUTFILE "index.out"
using namespace std;
void output(char *name,vect **data,int n,int dim)
{
   // cout << "Sending output to file \""<< name << "\"" << endl;
    ofstream out;
    out.open(name);
    for(int i = 0; i < n; i++)
    {
        for(int j = 0; j < dim; j++)
            out << data[i]->value(j) << "\t";
        out << endl;
    }
    out.close();
}

///////////// changed /////////////////////////////////////////////
// replace vector<string> q, with: int q[]
int find_word(int q[],vect **words, int n, int dim, vect w)
{
    int winner = 0;
    double dist;

    double sqsum = 0;
    for(int i = 0; i < dim; i++)
        sqsum += pow(words[0]->value(i) - w.value(i),2);
    dist = sqrt(sqsum);
    for(int i = 1; i < n; i++)
    {
        sqsum = 0;
        for(int j = 0; j < dim; j++)
            sqsum += pow(words[i]->value(j) - w.value(j),2);
        if(dist > sqrt(sqsum))
        {
            // new winner
            dist = sqrt(sqsum);
            winner = i;
        }
    }
    return q[winner];
}
//////////////////////////////////////////////////////////////////////


int main(int argc, char **argv)
{
    if(argc < 7)
    {
        cerr << "Usage: " << endl;
        cerr << argv[0] << " <start_time> <time_step> <num_steps> <dim> <x1(t0)> ... <xn(t0)> <amc file>" << endl;
        exit(EXIT_FAILURE);
    }

    if((argc-6) != atoi(argv[4]))
    {
        cerr << "ARGUMENTS TO NOT MATCH DIMENSION!" << endl;
        exit(EXIT_FAILURE);
    }
    
    cout << "#!FILE PROCESSED" << endl;
    cout << ":FULLY-SPECIFIED" << endl;
    cout << ":DEGREES" << endl;
    
    double t0 = atof(argv[1]);
    double h = atof(argv[2]);
    int n = atoi(argv[3]);
    int dim = atoi(argv[4]);

    vect **data = new vect* [n];
    for (int i = 0; i < n; i++)
	    data[i] = new vect(dim);

    vect x(atoi(argv[4]));
    for(int i = 0; i < x.size(); i++)
        x.assign(atof(argv[i+5]),i);

    vect nx(x.size());

    for(int i = 0; i < n;i++)
    {
        
        *data[i]=x;
        nx = runge(x,h,t0);
        t0+=h;
        
        x = nx;
    }
    output(DATA_NAME,data,n,dim);

    // now lorenz is in vect data and points are evenly spaced with time
    // input file, and count words
    
    /////////////// load file into vector ////
    // stuff AMC data into an array //
    // save indexes in vector //
    // after shuffleing, use indicies in //
    // vector to access array AMC data //
    //////////////////////////////////////////
 //   cout << "file = " << argv[5+dim] << endl;
    amc inter_data(argv[5+dim]);
    
    ////////////////////////////////////////
    
    // build array with indexes;
    int* amc_index = new int [inter_data.index()];
    for(int i = 0; i < inter_data.index(); i++)
    {
        amc_index[i] = i;
    }
    /////////////////////////////////////////
    
    int p = 0;
    ///////////////////////////////// need to change *******************
    // now spread points over plot
   //// cout << "Spreading data" << endl;
   // vect *words = new vect [q.size()] (dim);
   // for(int i = 0; i < (int)q.size() * (int)(n/q.size());i++)
   // {
    //    if(i%(int)(n/q.size()) == 0)
   //         words[p++] = data[i];
   // }
    vect **words = new vect *[inter_data.index()];
    for(int i = 0; i < inter_data.index(); i++)
	    words[i] = new vect(dim);
    for(int i = 0; i < (int)inter_data.index() * (int)(n/inter_data.index());i++)
    {
       if(i%(int)(n/inter_data.index()) == 0)
            *words[p++] = *data[i];
    }
    
   output("data_d",words,inter_data.index(),dim);
    //////////////////////////////////////////////////////////////////
    

   // cout << "Enter new I.C's:" << endl;
    vect w(dim);
    for(int i = 0; i < dim; i++)
    {
        cerr << "x["<<i<<"] = ";
        double t;
        cin >> t;
        w.assign(t,i);
    }
    
    /////////// remove crap so i can just redirect from stdout ///////////////
    //cout << "Output file for text : ";
    //ofstream out;
   // string fname;
    //cin >> fname;
    //out.open(fname.c_str());
   // cout << "Calculating new tracetories ..." << endl;
   //////////////////////////////////////////////////////////////////////////
    vect nw(dim);
    int amcIndexCount = 1;
    //////////////// changed /////////////////////////////////////////
    ofstream indexout;
    indexout.open(INDEX_OUTFILE);
    
    double tn = atof(argv[1]);
  //  int _t = 100000;
    for(int i = 0; i < (int)inter_data.index() * (int)(n/inter_data.index());i++) //_t;i++)//(int)inter_data.index() * (int)(n/inter_data.index());i++) // length of output??
    {
        if(i%(int)(n/inter_data.index()) == 0) // whats this?
        {
            // changed call ////////////////////////////////////
            // out << find_word(q,words,q.size(),dim,w) << " ";
            cout << amcIndexCount++ << endl;
            int _index = find_word(amc_index,
                                                  words,        // right?
                                                  inter_data.index(),
                                                  dim,          // right?
                                                  w
                                  ); // right?
            inter_data.dumpindex(_index);
            indexout << _index << endl; // dumps DATA STRUCTURE INDICIES (frames-1)!!!
        }
            ////////////////////////////////////////////////////
        nw = runge(w,h,tn);
        tn+=h;
        w = nw;
    }
    //////////////////////////////////////////////////////////////////////
    indexout.close();
    
   // out << endl;
    cerr << "Success!\n";
    
    return 0;
}
