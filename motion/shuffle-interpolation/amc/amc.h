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

#define MAX_ROOT	6
#define MAX_LOWERBACK	3
#define MAX_UPPERBACK	3
#define MAX_THORAX	3
#define MAX_LOWERNECK	3
#define MAX_UPPERNECK	3
#define MAX_HEAD	3
#define MAX_RCLAVICLE	2
#define MAX_RHUMERUS	3
#define MAX_RHAND	2
#define MAX_RTHUMB	2
#define MAX_LCLAVICLE	2
#define MAX_LHUMERUS	3
#define MAX_LHAND	2
#define MAX_LTHUMBS	2
#define MAX_RFEMUR	3
#define MAX_RFOOT	2
#define MAX_LFEMUR	3
#define MAX_LFOOT	2

using namespace std;
struct amc_data
{
	double root[MAX_ROOT];
	double lowerback[MAX_LOWERBACK];
	double upperback[MAX_UPPERBACK];
	double thorax[MAX_THORAX];
	double lowerneck[MAX_LOWERNECK];
	double upperneck[MAX_UPPERNECK];
	double head[MAX_HEAD];
	double rclavicle[MAX_RCLAVICLE];
	double rhumerus[MAX_RHUMERUS];
	double rradius;
	double rwrist;
	double rhand[MAX_RHAND];
	double rfingers;
	double rthumb[MAX_RTHUMB];
	double lclavicle[MAX_LCLAVICLE];
	double lhumerus[MAX_LHUMERUS];
	double lradius;
	double lwrist;
	double lhand[MAX_LHAND];
	double lfingers;
	double lthumb[MAX_LTHUMBS];
	double rfemur[MAX_RFEMUR];
	double rtibia;
	double rfoot[MAX_RFOOT];
	double rtoes;
	double lfemur[MAX_LFEMUR];
	double ltibia;
	double lfoot[MAX_LFOOT];
	double ltoes;
};

class amc
{
public:
	amc (char *filename);

	// splice constructor
	// splices second onto first (splice means it shits the root values over)
	amc (amc first, amc second);

	amc () {};

	void dumpall ();
	void dumpindex (int index);

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
