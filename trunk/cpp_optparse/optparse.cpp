/*
 * Implementation of optparse.h
 *
 * W. Evan Sheehan <evan_sheehan@nrel.gov>
 *
 */

#include "optparse.h"

#include <iostream>
#include <unistd.h>

using namespace std;

/********************** Option **********************/

Option::Option (string shrt, string lng, string dest):
	shrt_flag (shrt),
	lng_flag (lng),
	destination (dest)
{
}

Option::~Option ()
{
}

bool
Option::operator == (const string & option)
{
	return (shrt_flag == option || lng_flag == option);
}

/******************** OptionParser *******************/

OptionParser::OptionParser (string usage):
	use_msg (usage)
{
	opts.insert(opts.begin(), Option("-h","--help",""));
}

OptionParser::~OptionParser ()
{
}

void
OptionParser::add_option (string shrt_flag, string lng_flag, string destination, string dfault)
{
	opts.insert(opts.begin(),Option(shrt_flag, lng_flag, destination));
	options[destination] = dfault;
}

void
OptionParser::parse_args (int argc, char **argv)
{
	for (int i = 1; i < argc; i++) {
		if (argv[i][0] == '-') {
			find_opt(argv, i);
			i++;
		}
		else
			arguments.insert(arguments.begin(), argv[i]);
		//free(argv[i]);
	}
}

void
OptionParser::error (string msg)
{
	cerr << use_msg << endl << msg << endl;
	exit(1);
}

void
OptionParser::help ()
{
	cout << use_msg << endl;
	exit(0);
}

void
OptionParser::find_opt(char **argv, int index)
{
	for (int i = 0; i < opts.size(); i++) {
		if (opts[i] == (string)argv[index]) {
			options[opts[i].destination] = argv[index+1];
			return;
		}
	}

	error("Error: invalid argument");
}
/*** The End ***/

