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
	/* help option is default, it has no destination. */
	opts.insert(opts.begin(), Option("-h","--help",""));
}

OptionParser::~OptionParser ()
{
}

void
OptionParser::add_option (string shrt_flag, string lng_flag, string destination, string dfault)
{
	/* Add the option to our list of options. */
	opts.insert(opts.begin(),Option(shrt_flag, lng_flag, destination));

	/* Set the default value for this option, this not only allows you to
	 * set default values but insures that every option's destination will
	 * be in our dictionary, even if the value is only "".
	 */
	options[destination] = dfault;
}

void
OptionParser::parse_args (int argc, char **argv)
{
	/* Walk through the arguments and sort them into options
	 * or arguments.
	 */
	for (int i = 1; i < argc; i++) {
		/* If we start with a '-' we're probably a <flag><value> pair
		 * so we need to figure out which option it is. We increment i here
		 * because we assume that each flag has a value associated with it
		 * so we're interested in argv[i] and argv[i+1] and we don't want to
		 * count the value as both an option value and an argument.
		 * FIXME: this is not gonna work when we start have flags with no
		 * value (i.e. boolean flags)
		 */
		if (argv[i][0] == '-') {
			find_opt(argv, i);
			i++;
		}
		/* If we're looking at an argument (i.e. a value with no flag who's
		 * meaning is determined by position) just append it into the
		 * arguments list.
		 */
		else
			arguments.insert(arguments.end(), argv[i]);
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
	/* If it's the built in help option we'll just print the help message
	 * and quit.
	 */
	if ((string)argv[index] == "-h" || (string)argv[index] == "--help")
		help();

	/* Step through our list of known options. */
	for (int i = 0; i < opts.size(); i++) {
		/* Uses the overridden == operator for the Options class
		 * to compare argv[index] to the flags of each Option.
		 */
		if (opts[i] == (string)argv[index]) {
			/* Set the value and return if we've found a match. */
			options[opts[i].destination] = argv[index+1];
			return;
		}
	}

	/* If we haven't found a match this is not a known argument. */
	error("Error: invalid argument");
}
/*** The End ***/

