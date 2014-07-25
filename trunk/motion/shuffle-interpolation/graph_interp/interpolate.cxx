///////////////////////////////////////////////////
// file : main.cxx
// author: pclary
// description: builds graphs using amc_graph 
// and amc classes
////////////////////////////////////////////////////////

using namespace std;

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <vector>
#include "conversion.h"
#include "../amc/amc.h"
#include "../amc_graph/amc_graph.h"
#include "../graph_node/graph_node.h"

// how many bones

struct transition
{
	int index; // index of a
	int a;     // from index a
	int b;     // to index b
};

vector <transition> find_transitions(char *index_file)
{
	vector <transition> trans;

	ifstream in;
	in.open(index_file);
	if (in.fail ()) {
		cerr << "Error opening index.out file" << endl;
	}

	transition t;

	in >> t.a;
	t.index = 0;
	do {
		in >> t.b;
		if (t.a != (t.b-1) && t.a-t.b != 0) {
			trans.push_back(t);
		}
		t.index++;
		t.a = t.b;
	} while (in.peek () != EOF);

	in.close ();

	return trans;
}

vector <vector <int> > path_finder(vector <vector <int> > pinfo,
                                         vector <int> current_path,
                                         int gendidx,
                                         amc_graph agraph,
                                         graph_node current_node
                                         );
amc interpolate(amc adata, vector <amc_graph> graph_data, int startidx, int endidx);

int main(int argc, char **argv)
{
	if(argc != 4) {
		cerr << "usage: interpolate index.out original_amc shuffled_amc\n";
		exit (EXIT_FAILURE);
	};

	// read in transitions from file
	cerr << "finding transitions ..." << endl;
	vector <transition> trans;
	trans = find_transitions(argv[1]);

	// read in amc files from command line
	cerr << "getting amc files ..." << endl;
	amc original_amc(argv[2]);
	amc shuffled_amc(argv[3]);

	// convert all data of original
	cerr << "converting all data ..." << endl;
	vector <double *> *data;
	data = convert_all(original_amc);

	// create graphs of original
	cerr << "creating all graphs ..." << endl;
	vector <amc_graph> agraphs;
	agraphs = create_all_graphs(data);

	// print graph
	// agraphs[1].print_graph();

	// interpolate and concatenate
	cerr << "interpolating and concatenating ..." << endl;
	amc result_amc;
	// FIXME: is +1 correct?
	result_amc.num = trans[0].index+1;
	result_amc.a = new amc_data [result_amc.num];
	//result_amc.a = new amc_data [shuffled_amc.num + (trans.size() * 120) + 10];

	// for debugging
	cerr << "space allocated ...  " << endl;
	// cerr << "working ... ";

	// copy over first part
	for (int i = 0; i < result_amc.num; i++) {
		result_amc.a[i] = shuffled_amc.a[i];
	}

	for (unsigned int transitions = 0; transitions < trans.size(); transitions++) {
		double perc = (double)((double)transitions/(double)trans.size()) * 100.0;
		cerr << "*** " << perc << "% complete" << endl;

		// interpolate
		//FIXME: is -1 correct?
		amc next_amc = interpolate(original_amc,agraphs,trans[transitions].a-1,trans[transitions].b-1);

		// three pieces need to go together
		// the old result + interpolation + new chunk of shuffled

		// get new chunk
		amc next_chunk;
		if (transitions == trans.size()-1) {
			next_chunk.num = (shuffled_amc.num-1) - trans[transitions].index;
			next_chunk.a = shuffled_amc.a+trans[transitions].index+1;
		} else {
			next_chunk.num = (trans[transitions+1].index-(trans[transitions].index));
			next_chunk.a = shuffled_amc.a+trans[transitions].index+1;
		}

		amc temp(result_amc,next_amc);
		amc temp2(temp,next_chunk);
		result_amc.num = temp2.num;
		result_amc.a = temp2.a;
	}
	cerr << endl;

	// dump results
	cerr << "output results ..." << endl;
	cout << "#!FILE PROCESSED" << endl;
	cout << ":FULLY-SPECIFIED" << endl;
	cout << ":DEGREES" << endl;
	result_amc.dumpall();

	cerr << "Success." << endl;
	return EXIT_SUCCESS;
}


vector < vector <int> >
path_finder(vector< vector<int> > pinfo,
	    vector <int> current_path,
	    int gendidx,
	    amc_graph agraph,
	    graph_node current_node)
{
	// idea, keep a flag to say if it hit target, (or if not yes-run-out-of-edges)
	// only push if this true
	// path_info . isgood - but might want to push anyway, so outer function
	// can tell is a path is possible

	// push on current_path
	current_path.push_back(current_node.name);

	// if hit the goal, a finishing condition
	if (current_node.name == gendidx) {
		pinfo.push_back(current_path);
		//cerr << "\t\t*path found" << endl;
		return pinfo;
	}

	// for each edge
	for (unsigned int EDGE = 0; EDGE < current_node.edges.size(); EDGE++) {
		// if this node is in current path.. its a loop, dont go there
		bool isloop = false; // innocent until proven guilty
		for (unsigned int i = 0; i < current_path.size(); i++) {
			if (current_path[i] == current_node.edges[EDGE].to_node_id) {
				isloop = true;
				break;
			}
		}

		if (!isloop) {
			// only recurse if no loop
			// get next node id
			int nodeidx;
			for (unsigned int i = 0; i < agraph.nodes.size(); i++) {
				if (agraph.nodes[i].name == current_node.edges[EDGE].to_node_id) {
					nodeidx = i;
					break;
				}
			}

			vector < vector <int> > pres = path_finder(pinfo, current_path, gendidx, agraph, agraph.nodes[nodeidx]);
			// add onto stack
			//if(pres.size() > 0)
			for (unsigned int i = 0; i < pres.size(); i++) {
				pinfo.push_back(pres[i]);

				//return pinfo;
			}
			//if(pres.size() > 0)
			//	return pinfo;

		}
	}

	// might need another recursive function
	// for each path?
	// THINK ABOUT THIS!!

	return pinfo; /******* might need some flag to say if this hit the target, NO if size() == 0 for a graph, then it didnt ******/
}


//////////////////////////////////////////////////////////////////////////////
// interpolate() function: takes the amc file used for interpolation,
// a graph of that amc file, the start index of the amc for interpolation,
// and the end of the index of interpolation. the function returns and amc
// structure of the interpolated area.
//////////////////////////////////////////////////////////////////////////////
amc interpolate (amc adata, vector <amc_graph> graph_data, int startidx, int endidx)
{
	// check that start and end are not the same
	if (startidx == endidx) {
		cerr << "ERROR: Trying to inpterolate over same frame" << endl;
		exit (EXIT_FAILURE);
	}

	// find startidx and endidx in graphs
	/*
	vector <int> gstartidx; // array of, int for each graph
	vector <int> gendidx;
	for (unsigned int i = 0; i < graph_data.size (); i++) {
		// iterate over each graph
		if (graph_data[i].nodes.size ()) {
			cerr << "interpolate: ERROR graph : " << i << ": is empty! quitting..." << endl;
			exit (EXIT_FAILURE);
		}
		for (unsigned int j = 0; j < graph_data[i].nodes.size (); j++) {
			// iterate over each node
			if(graph_data[i].nodes[j].name == startidx) {
				// found start node
				gstartidx.push_back(j);
			} else if(graph_data[i].nodes[j].name == endidx) {
				// found end node
				gendidx.push_back(j);
			}
		}
	}
	*/

	/*
	 * MAJOR PROBLEM:
	 * endidx wont be found.. because the frames are all consolidated into nodes
	 * with the name of the first frame that hits it..
	 *
	 * solution:
	 * find which node this frame WOULD be in.. maybe work with graph indecies
	 * instead of amc indices
	 *
	 *
	 * CHECK ALLNAME (NODE) FIELD
	 *
	 * get endidx that exists in a node
	 */

	// find all paths for each graph from start to end
	// be careful of self loops, not to go to iterate forever
	vector <vector < vector <int> > > path; // push paths onto this stack
	cerr << "finding paths .. " << endl;
	for (unsigned int each_graph = 0; each_graph < graph_data.size(); each_graph++) {
		// can start on graph 1, because 0 is root, and we're not interpolating it the same
		// each graph
		// must use 0 so path.push_backs fill it
		cerr <<each_graph << ",";
		if(graph_data[each_graph].nodes.size() == 0) {
			cerr << "interpolate: ERROR graph : " << each_graph << ": is empty! quitting..." << endl;
			exit (EXIT_FAILURE);
		}

		// get endidx and startidx that exists in a node
		int rendidx;
		int rstartidx;
		for (unsigned int i = 0; i < graph_data[each_graph].nodes.size (); i++) {
			// for each node
			for (unsigned int j = 0; j < graph_data[each_graph].nodes[i].allnames.size (); j++) {
				if (endidx == graph_data[each_graph].nodes[i].allnames[j]) {
					rendidx = graph_data[each_graph].nodes[i].name;
				}
				if(startidx == graph_data[each_graph].nodes[i].allnames[j]) {
					rstartidx = graph_data[each_graph].nodes[i].name;
				}
			}
		}

		vector < vector <int> > graph_path; // collection of paths for a single graph

		// recursive function needed
		vector <int> current_path; // push current path onto this stack
					   // once end is found, push onto graph_path stack
		//find starting node
		int graphidx;
		for (int INODE = 0; INODE < (int) graph_data[each_graph].nodes.size (); INODE++) {
			if (graph_data[each_graph].nodes[INODE].name == rstartidx) {
				graphidx = INODE;
				break;
			}
		}

		graph_path = path_finder (graph_path,
					  current_path,
					  rendidx,
					  graph_data[each_graph],
					  graph_data[each_graph].nodes[graphidx]);


		path.push_back(graph_path);
		// this way different things can be done if path is not successful
		// (no good path) isgood wont be true, or path will be empty
	}
	cerr << " done." << endl;

	//dump path, for debugging
	/*
	for (unsigned int graph = 0; graph < path.size (); graph++) {
		cout << "graph " << graph << ":" << endl;
		for (unsigned int cpath = 0; cpath < path[graph].size (); cpath++) {
			for (unsigned int i = 0; i < path[graph][cpath].size (); i++) {
				cout << path[graph][cpath][i] << ", ";
			}
			cout << endl;
		}
	}
	*/

	// calculate greatest-least path for the graphs
	// this is the smallest value of frames for a single graph,
	// but the largest of these small values over all the graphs
	//
	// is g-l what we want to do?
	// wouldnt it be better for them all just to be the same?
	//////////////
	cerr << "calculating greatest-least path" << endl;
	unsigned int greatest_least = 0;
	for (unsigned int each_bone = 0; each_bone < path.size (); each_bone++) {
		if (path[each_bone].size () == 0) {
			vector <int> ttp;
			ttp.push_back(startidx);
			path[each_bone].push_back(ttp);
			// no path exists!! linear interpolation?
			cerr << "interpolation: no path exists for bone: " << each_bone << " self loop" << endl;//quitting.. " << endl;
		}
	}
	greatest_least = 100;
	cerr << "greatest-least = " << greatest_least << endl;
	cerr << "calculating path to use ... " << endl;
	// use paths that are closest to this value of frames for each graph
	vector <unsigned int> path_to_use;
	unsigned int amc_size = 0;
	for (unsigned int each_bone = 0; each_bone < path.size (); each_bone++) {
		unsigned int winner = 0;
		for (unsigned int each_graph = 0; each_graph < path[each_bone].size (); each_graph++) {
			if (pow ((double)((int)greatest_least - (int)path[each_bone][each_graph].size ()),2) <
			    pow ((double)((int)greatest_least - (int)path[each_bone][winner].size ()),2)) {
				winner = each_graph;
			}
		}
		cerr << each_bone << ",";
		path_to_use.push_back(0); //winner);
		// also find greatest size
		cerr << "b";
		if (path[each_bone].size() > 0 && path[each_bone][winner].size() > amc_size) {
			amc_size = path[each_bone][winner].size();
		}
		cerr << "e";
	}
	cerr << endl;

	// use these sequences to build new amc file
	// if certain body parts finish before others, simply insert self loops.
	// THIS IS WHERE BODY PART DEPENDENCIES SHOULD BE ADDED, IF EVER,
	// OR THEY COULD BE SIMPLY BUILT INTO THE GRAPHS

	/*****************
	// dont interpolate root position,
	// do linear interpolation of facing... with same time as amc_size
	//
	// todo:
	// for shuffle, shift root location!, (like splice) on each shuffle
	******************/

	// for debugging
	//int amc_size = 120;

	amc new_amc;
	new_amc.num = amc_size;
	new_amc.a = new amc_data [amc_size];

	cerr << "creating frames ..." << endl;
	for (unsigned int frame = 0; frame < amc_size; frame++) {
		cerr << frame << ",";
		/////////////////////////////////////
		// insert self loop if already done
		/////////////////////////////////////

		//   cerr << "frame = " << frame << endl;
		// else, copy indexes from amc file
		// need an (else) after each if statement
		// use indexes found and adata to build new amc file


		/************* file dependant section *************/
		/**** (name) problem ****/
		// start at 1, 0 is root

		// lowerback	MAX_LOWERBACK
		if (frame < path[1][path_to_use[1]].size()) {
			// still good
			for (int i = 0; i < MAX_LOWERBACK; i++) {
				new_amc.a[frame].lowerback[i] = adata.a[path[1][path_to_use[1]][frame]].lowerback[i];
			}
		} else {
			// self loops
			for (int i = 0; i < MAX_LOWERBACK; i++) {
				new_amc.a[frame].lowerback[i] = new_amc.a[frame-1].lowerback[i];
			}
		}

		// upperback	MAX_UPPERBACK
		if (frame < path[2][path_to_use[2]].size()) {
			// still good
			for (int i = 0; i < MAX_UPPERBACK; i++) {
				new_amc.a[frame].upperback[i] = adata.a[path[2][path_to_use[2]][frame]].upperback[i];
			}
		} else {
			// self loops
			for (int i = 0; i < MAX_UPPERBACK; i++) {
				new_amc.a[frame].upperback[i] = new_amc.a[frame-1].upperback[i];
			}
		}

		// thorax	MAX_THORAX
		if (frame < path[3][path_to_use[3]].size()) {
			// still good
			for (int i = 0; i < MAX_THORAX; i++) {
				new_amc.a[frame].thorax[i] = adata.a[path[3][path_to_use[3]][frame]].thorax[i];
			}
		} else {
			// self loops
			for (int i = 0; i < MAX_THORAX; i++) {
				new_amc.a[frame].thorax[i] = new_amc.a[frame-1].thorax[i];
			}
		}


		// lowerneck	MAX_LOWERNECK
		if (frame < path[4][path_to_use[4]].size()) {
			// still good
			for (int i = 0; i < MAX_LOWERNECK; i++) {
				new_amc.a[frame].lowerneck[i] = adata.a[path[4][path_to_use[4]][frame]].lowerneck[i];
			}
		} else {
			// self loops
			for (int i = 0; i < MAX_LOWERNECK; i++) {
				new_amc.a[frame].lowerneck[i] = new_amc.a[frame-1].lowerneck[i];
			}
		}


		// upperneck	MAX_UPPERNECK
		if (frame < path[5][path_to_use[5]].size()) {
			// still good
			for (int i = 0; i < MAX_UPPERNECK; i++) {
				new_amc.a[frame].upperneck[i] = adata.a[path[5][path_to_use[5]][frame]].upperneck[i];
			}
		} else {
			// self loops
			for(int i = 0; i < MAX_UPPERNECK; i++) {
				new_amc.a[frame].upperneck[i] = new_amc.a[frame-1].upperneck[i];
			}
		}


		// head	MAX_HEAD
		if (frame < path[6][path_to_use[6]].size()) {
			// still good
			for (int i = 0; i < MAX_HEAD; i++) {
				new_amc.a[frame].head[i] = adata.a[path[6][path_to_use[6]][frame]].head[i];
			}
		} else {
			// self loops
			for (int i = 0; i < MAX_HEAD; i++) {
				new_amc.a[frame].head[i] = new_amc.a[frame-1].head[i];
			}
		}


		// rclavicle	MAX_RCLAVICLE
		if (frame < path[7][path_to_use[7]].size()) {
			// still good
			for (int i = 0; i < MAX_RCLAVICLE; i++) {
				new_amc.a[frame].rclavicle[i] = adata.a[path[7][path_to_use[7]][frame]].rclavicle[i];
			}
		} else {
			// self loops
			for (int i = 0; i < MAX_RCLAVICLE; i++) {
				new_amc.a[frame].rclavicle[i] = new_amc.a[frame-1].rclavicle[i];
			}
		}


		// rhumerus	MAX_RHUMERUS
		if(frame < path[8][path_to_use[8]].size()) {
			// still good
			for (int i = 0; i < MAX_RHUMERUS; i++) {
				new_amc.a[frame].rhumerus[i] = adata.a[path[8][path_to_use[8]][frame]].rhumerus[i];
			}
		} else {
			// self loops
			for (int i = 0; i < MAX_RHUMERUS; i++) {
				new_amc.a[frame].rhumerus[i] = new_amc.a[frame-1].rhumerus[i];
			}
		}


		// rradius	ONE
		if (frame < path[9][path_to_use[9]].size()) {
			// still good
			new_amc.a[frame].rradius = adata.a[path[9][path_to_use[9]][frame]].rradius;
		} else {
			// self loops
			new_amc.a[frame].rradius = new_amc.a[frame-1].rradius;
		}


		// rwrist	ONE
		if (frame < path[10][path_to_use[10]].size()) {
			// still good
			new_amc.a[frame].rwrist = adata.a[path[10][path_to_use[10]][frame]].rwrist;
		} else {
			// self loops
			new_amc.a[frame].rwrist = new_amc.a[frame-1].rwrist;
		}


		// rhand	MAX_RHAND
		if (frame < path[11][path_to_use[11]].size()) {
			// still good
			for (int i = 0; i < MAX_RHAND; i++) {
				new_amc.a[frame].rhand[i] = adata.a[path[11][path_to_use[11]][frame]].rhand[i];
			}
		} else {
			// self loops
			for (int i = 0; i < MAX_RHAND; i++) {
				new_amc.a[frame].rhand[i] = new_amc.a[frame-1].rhand[i];
			}
		}


		// rfingers	ONE
		if (frame < path[12][path_to_use[12]].size()) {
			// still good
			new_amc.a[frame].rfingers = adata.a[path[12][path_to_use[12]][frame]].rfingers;
		} else {
			// self loops
			new_amc.a[frame].rfingers = new_amc.a[frame-1].rfingers;
		}


		// rthumb	MAX_RTHUMB
		if (frame < path[13][path_to_use[13]].size()) {
			// still good
			for (int i = 0; i < MAX_RTHUMB; i++) {
				new_amc.a[frame].rthumb[i] = adata.a[path[13][path_to_use[13]][frame]].rthumb[i];
			}
		} else {
			// self loops
			for (int i = 0; i < MAX_RTHUMB; i++) {
				new_amc.a[frame].rthumb[i] = new_amc.a[frame-1].rthumb[i];
			}
		}


		// lclavicle	MAX_LCLAVICLE
		if (frame < path[14][path_to_use[14]].size()) {
			// still good
			for (int i = 0; i < MAX_LCLAVICLE; i++) {
				new_amc.a[frame].lclavicle[i] = adata.a[path[14][path_to_use[14]][frame]].lclavicle[i];
			}
		} else {
			// self loops
			for (int i = 0; i < MAX_LCLAVICLE; i++) {
				new_amc.a[frame].lclavicle[i] = new_amc.a[frame-1].lclavicle[i];
			}
		}


		// lhumerus	MAX_LHUMERUS
		if (frame < path[15][path_to_use[15]].size()) {
			// still good
			for (int i = 0; i < MAX_LHUMERUS; i++) {
				new_amc.a[frame].lhumerus[i] = adata.a[path[15][path_to_use[15]][frame]].lhumerus[i];
			}
		} else {
			// self loops
			for(int i = 0; i < MAX_LHUMERUS; i++) {
				new_amc.a[frame].lhumerus[i] = new_amc.a[frame-1].lhumerus[i];
			}
		}


		// lradius	ONE
		if (frame < path[16][path_to_use[16]].size()) {
			// still good
			new_amc.a[frame].lradius = adata.a[path[16][path_to_use[16]][frame]].lradius;
		} else {
			// self loops
			new_amc.a[frame].lradius = new_amc.a[frame-1].lradius;
		}


		// lwrist	ONE
		if (frame < path[17][path_to_use[17]].size()) {
			// still good
			new_amc.a[frame].lwrist = adata.a[path[17][path_to_use[17]][frame]].lwrist;
		} else {
			// self loops
			new_amc.a[frame].lwrist = new_amc.a[frame-1].lwrist;
		}


		// lhand	MAX_LHAND
		if (frame < path[18][path_to_use[18]].size()) {
			// still good
			for (int i = 0; i < MAX_LHAND; i++) {
				new_amc.a[frame].lhand[i] = adata.a[path[18][path_to_use[18]][frame]].lhand[i];
			}
		} else {
			// self loops
			for (int i = 0; i < MAX_LHAND; i++) {
				new_amc.a[frame].lhand[i] = new_amc.a[frame-1].lhand[i];
			}
		}


		// lfingers	ONE
		if (frame < path[19][path_to_use[19]].size()) {
			// still good
			new_amc.a[frame].lfingers = adata.a[path[19][path_to_use[19]][frame]].lfingers;
		} else {
			// self loops
			new_amc.a[frame].lfingers = new_amc.a[frame-1].lfingers;
		}


		// lthumb	MAX_LTHUMBS
		if (frame < path[20][path_to_use[20]].size()) {
			// still good
			for (int i = 0; i < MAX_LTHUMBS; i++) {
				new_amc.a[frame].lthumb[i] = adata.a[path[20][path_to_use[20]][frame]].lthumb[i];
			}
		} else {
			// self loops
			for(int i = 0; i < MAX_LTHUMBS; i++) {
				new_amc.a[frame].lthumb[i] = new_amc.a[frame-1].lthumb[i];
			}
		}


		// rfemur	MAX_RFEMUR
		if (frame < path[21][path_to_use[21]].size()) {
			// still good
			for (int i = 0; i < MAX_RFEMUR; i++) {
				new_amc.a[frame].rfemur[i] = adata.a[path[21][path_to_use[21]][frame]].rfemur[i];
			}
		} else {
			// self loops
			for(int i = 0; i < MAX_RFEMUR; i++) {
				new_amc.a[frame].rfemur[i] = new_amc.a[frame-1].rfemur[i];
			}
		}


		// rtibia	ONE
		if (frame < path[22][path_to_use[22]].size()) {
			// still good
			new_amc.a[frame].rtibia = adata.a[path[22][path_to_use[22]][frame]].rtibia;
		} else {
			// self loops
			new_amc.a[frame].rtibia = new_amc.a[frame-1].rtibia;
		}


		// rfoot	MAX_RFOOT
		if (frame < path[23][path_to_use[23]].size()) {
			// still good
			for (int i = 0; i < MAX_RFOOT; i++) {
				new_amc.a[frame].rfoot[i] = adata.a[path[23][path_to_use[23]][frame]].rfoot[i];
			}
		} else {
			// self loops
			for(int i = 0; i < MAX_RFOOT; i++) {
				new_amc.a[frame].rfoot[i] = new_amc.a[frame-1].rfoot[i];
			}
		}


		// rtoes	ONE
		if(frame < path[24][path_to_use[24]].size()) {
			// still good
			new_amc.a[frame].rtoes = adata.a[path[24][path_to_use[24]][frame]].rtoes;
		} else { // self loops
			new_amc.a[frame].rtoes = new_amc.a[frame-1].rtoes;
		}


		// lfemur	MAX_LFEMUR
		if(frame < path[25][path_to_use[25]].size()) {
			// still good
			for(int i = 0; i < MAX_LFEMUR; i++) {
				new_amc.a[frame].lfemur[i] = adata.a[path[25][path_to_use[25]][frame]].lfemur[i];
			}
		} else { // self loops
			for(int i = 0; i < MAX_LFEMUR; i++) {
				new_amc.a[frame].lfemur[i] = new_amc.a[frame-1].lfemur[i];
			}
		}


		// ltibia	ONE
		if(frame < path[26][path_to_use[26]].size()) {
			// still good
			new_amc.a[frame].ltibia = adata.a[path[26][path_to_use[26]][frame]].ltibia;
		} else { // self loops
			new_amc.a[frame].ltibia = new_amc.a[frame-1].ltibia;
		}


		// lfoot	MAX_LFOOT
		if(frame < path[27][path_to_use[27]].size()) {
			// still good
			for(int i = 0; i < MAX_LFOOT; i++) {
				new_amc.a[frame].lfoot[i] = adata.a[path[27][path_to_use[27]][frame]].lfoot[i];
			}
		} else { // self loops
			for(int i = 0; i < MAX_LFOOT; i++) {
				new_amc.a[frame].lfoot[i] = new_amc.a[frame-1].lfoot[i];
			}
		}


		cerr << "b";
		// ltoes	ONE
		if(frame < path[28][path_to_use[28]].size()) {
			// still good
			new_amc.a[frame].ltoes = adata.a[path[28][path_to_use[28]][frame]].ltoes;
		} else { // self loops
			new_amc.a[frame].ltoes = new_amc.a[frame-1].ltoes;
		}

		cerr << "e";

		/************* end: file dependant section *************/
	}

	cerr << endl;

        cerr << "root interpolation ... " << endl;
	// interpolate root orientation
	for (unsigned int amc_size_step = 0; amc_size_step < amc_size; amc_size_step++) {
		// root orientation is last three numbers
		for (int each_root = 3; each_root < MAX_ROOT; each_root++) {
			// check if this is right
			double total_size = adata.a[endidx].root[each_root] - adata.a[startidx].root[each_root];
			double step_size = total_size/amc_size;
			new_amc.a[amc_size_step].root[each_root] = step_size*amc_size_step + adata.a[startidx].root[each_root];
		}
	}

	cerr << " new sequence size = " << amc_size << endl;;

	return new_amc;
}

