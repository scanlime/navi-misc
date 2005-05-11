//////////////////////////////////
// AUTHOR: PATRICK CLARY
// DESCRIPTION:
// defines structure used to
// save .amc file data structure
// for motion capture .amc files
///////////////////////////////////

#include <stdlib.h>
#include <string>
//#include "../graph/graph.h"

#ifndef AMC_H
#define AMC_H

using namespace std;
struct amc_data
{
#define MAX_ROOT 6
    double root[MAX_ROOT]; 

#define MAX_LOWERBACK 3
    double lowerback[MAX_LOWERBACK];

#define MAX_UPPERBACK 3
    double upperback[MAX_UPPERBACK];

#define MAX_THORAX 3
    double thorax[MAX_THORAX];
    
#define MAX_LOWERNECK 3
    double lowerneck[MAX_LOWERNECK];
    
#define MAX_UPPERNECK 3
    double upperneck[MAX_UPPERNECK];
    
#define MAX_HEAD 3
    double head[MAX_HEAD];
    
#define MAX_RCLAVICLE 2
    double rclavicle[MAX_RCLAVICLE];
    
#define MAX_RHUMERUS 3
    double rhumerus[MAX_RHUMERUS];

    double rradius;
    double rwrist;
    
#define MAX_RHAND 2
    double rhand[MAX_RHAND];
    
    double rfingers;

#define MAX_RTHUMB 2
    double rthumb[MAX_RTHUMB];

#define MAX_LCLAVICLE 2
    double lclavicle[MAX_LCLAVICLE];

#define MAX_LHUMERUS 3
    double lhumerus[MAX_LHUMERUS];

    double lradius;
    double lwrist;
    
#define MAX_LHAND 2
    double lhand[MAX_LHAND];

    double lfingers;
    
#define MAX_LTHUMBS 2
    double lthumb[MAX_LTHUMBS];
    
#define MAX_RFEMUR 3
    double rfemur[MAX_RFEMUR];
    
    double rtibia;
    
#define MAX_RFOOT 2
    double rfoot[MAX_RFOOT];
    
    double rtoes;

#define MAX_LFEMUR 3
    double lfemur[MAX_LFEMUR];

    double ltibia;

#define MAX_LFOOT 2
    double lfoot[MAX_LFOOT];

    double ltoes;
};

class amc
{
public:
    // constructor reads in file to data structure
    amc(char *filename); // takes amc filename as argument
    
    // splice constructor
    // splices second onto first (splice means it shits the root values over)
    amc(amc first, amc second);
    
    amc()
    {
        // do nothing
    };
    
    // tbd... destructor
    
    void dumpall(); // dump all method
    void dumpindex(int index); // dump index method
    
    // ..tbd build for different parts
    //void buildgraph(int numnodes); // output graph file
                                   // for dot program
                                   // note: numnodes is the number of nodes
                                   // for a 1D graph. there will be more for
                                   // nD graphs.
    
    
    /**************************************************************/
    
   /* void creategraph_root(int numnodes, char *filename);
    
    void creategraph_lowerback(int numnodes, char *filename);
    void creategraph_upperback(int numnodes, char *filename);
    void creategraph_thorax(int numnodes, char *filename);
    void creategraph_lowerneck(int numnodes, char *filename);
    void creategraph_upperneck(int numnodes, char *filename);
    void creategraph_head(int numnodes, char *filename);
    void creategraph_rclavicle(int numnodes, char *filename);
    void creategraph_rhumerus(int numnodes, char *filename);
    */
    //void creategraph_rradius(int numnodes, char *filename);
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
   // void creategraph_ltoes(int numnodes, char *filename);
    
        
        
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
        
        
    int index()
    {
        return num;
    }
    
    int num;
    // max num of frames
#define MAX_AMC_DATA 500000
    amc_data *a;
};

#endif

